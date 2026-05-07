#include "../../../src/robot_hand/cli/CommandLine.hpp"
#include "../test_doubles/CommandMock.hpp"
#include "../test_doubles/SerialLineSourceStub.hpp"
#include "../test_doubles/SerialOutputMock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

namespace
{
    class CommandLineTest
        : public testing::Test
    {
    protected:
        CommandLineTest()
        {
            ON_CALL(firstCommand, Name()).WillByDefault(testing::Return("first"));
            ON_CALL(secondCommand, Name()).WillByDefault(testing::Return("second"));
            ON_CALL(thirdCommand, Name()).WillByDefault(testing::Return("third"));

            ON_CALL(firstCommand, Description()).WillByDefault(testing::Return("first description"));
            ON_CALL(secondCommand, Description()).WillByDefault(testing::Return("second description"));
            ON_CALL(thirdCommand, Description()).WillByDefault(testing::Return("third description"));
        }

        static std::vector<std::string_view> ToVector(infra::MemoryRange<const std::string_view> arguments)
        {
            return { arguments.begin(), arguments.end() };
        }

        SerialLineSourceStub lineSource;
        testing::StrictMock<SerialOutputMock> serialOutput;
        CommandLine<2, 3> commandLine{ lineSource, serialOutput };
        testing::NiceMock<CommandMock> firstCommand;
        testing::NiceMock<CommandMock> secondCommand;
        testing::NiceMock<CommandMock> thirdCommand;
    };
}

TEST_F(CommandLineTest, RegisterCommandReturnsTrueWhileSpaceIsAvailable)
{
    EXPECT_TRUE(commandLine.RegisterCommand(firstCommand));
    EXPECT_TRUE(commandLine.RegisterCommand(secondCommand));
}

TEST_F(CommandLineTest, RegisterCommandReturnsFalseWhenCapacityIsReached)
{
    ASSERT_TRUE(commandLine.RegisterCommand(firstCommand));
    ASSERT_TRUE(commandLine.RegisterCommand(secondCommand));

    EXPECT_FALSE(commandLine.RegisterCommand(thirdCommand));
}

TEST_F(CommandLineTest, GetCommandByNameReturnsRegisteredCommand)
{
    ASSERT_TRUE(commandLine.RegisterCommand(firstCommand));
    ASSERT_TRUE(commandLine.RegisterCommand(secondCommand));

    auto command = commandLine.GetCommand("second");

    ASSERT_TRUE(command.has_value());
    EXPECT_EQ(&command->get(), &secondCommand);
}

TEST_F(CommandLineTest, GetCommandByNameReturnsNulloptWhenCommandDoesNotExist)
{
    ASSERT_TRUE(commandLine.RegisterCommand(firstCommand));

    EXPECT_FALSE(commandLine.GetCommand("missing").has_value());
}

TEST_F(CommandLineTest, GetCommandByIndexReturnsRegisteredCommand)
{
    ASSERT_TRUE(commandLine.RegisterCommand(firstCommand));
    ASSERT_TRUE(commandLine.RegisterCommand(secondCommand));

    auto command = commandLine.GetCommand(1);

    ASSERT_TRUE(command.has_value());
    EXPECT_EQ(&command->get(), &secondCommand);
}

TEST_F(CommandLineTest, GetCommandByIndexReturnsNulloptWhenIndexIsOutOfRange)
{
    ASSERT_TRUE(commandLine.RegisterCommand(firstCommand));

    EXPECT_FALSE(commandLine.GetCommand(1).has_value());
}

TEST_F(CommandLineTest, RunRegistersLineHandler)
{
    EXPECT_FALSE(lineSource.HasLineHandler());
    EXPECT_FALSE(lineSource.HasByteHandler());

    commandLine.Run();

    EXPECT_TRUE(lineSource.HasLineHandler());
    EXPECT_FALSE(lineSource.HasByteHandler());
}

TEST_F(CommandLineTest, EmptyLineIsIgnored)
{
    commandLine.Run();

    EXPECT_CALL(firstCommand, Execute(testing::_, testing::_)).Times(0);
    EXPECT_CALL(serialOutput, Write(testing::A<std::string_view>())).Times(0);
    EXPECT_CALL(serialOutput, Write(testing::A<uint8_t>())).Times(0);

    lineSource.EmitLine("");
}

TEST_F(CommandLineTest, UnknownCommandWritesErrorMessage)
{
    commandLine.Run();

    testing::InSequence sequence;
    EXPECT_CALL(serialOutput, Write(std::string_view("Unknown command: "))).WillOnce(testing::Return(true));
    EXPECT_CALL(serialOutput, Write(std::string_view("missing"))).WillOnce(testing::Return(true));
    EXPECT_CALL(serialOutput, Write(std::string_view("\n"))).WillOnce(testing::Return(true));

    lineSource.EmitLine("missing arg1");
}

TEST_F(CommandLineTest, LeadingSpacesProduceEmptyCommandName)
{
    commandLine.Run();

    testing::InSequence sequence;
    EXPECT_CALL(serialOutput, Write(std::string_view("Unknown command: "))).WillOnce(testing::Return(true));
    EXPECT_CALL(serialOutput, Write(std::string_view(""))).WillOnce(testing::Return(true));
    EXPECT_CALL(serialOutput, Write(std::string_view("\n"))).WillOnce(testing::Return(true));

    lineSource.EmitLine("  first arg1");
}

TEST_F(CommandLineTest, RegisteredCommandExecutesWithNoArguments)
{
    ASSERT_TRUE(commandLine.RegisterCommand(firstCommand));
    commandLine.Run();

    EXPECT_CALL(serialOutput, Write(testing::A<std::string_view>())).Times(0);
    EXPECT_CALL(serialOutput, Write(testing::A<uint8_t>())).Times(0);
    EXPECT_CALL(firstCommand, Execute(testing::_, testing::_))
        .WillOnce([this](infra::MemoryRange<const std::string_view> arguments, ISerialOutput& output)
            {
                EXPECT_TRUE(arguments.empty());
                EXPECT_EQ(&output, &serialOutput);
            });

    lineSource.EmitLine("first");
}

TEST_F(CommandLineTest, RegisteredCommandExecutesWithParsedArguments)
{
    ASSERT_TRUE(commandLine.RegisterCommand(firstCommand));
    commandLine.Run();

    EXPECT_CALL(firstCommand, Execute(testing::_, testing::_))
        .WillOnce([this](infra::MemoryRange<const std::string_view> arguments, ISerialOutput& output)
            {
                EXPECT_THAT(ToVector(arguments), testing::ElementsAre("one", "two", "three"));
                EXPECT_EQ(&output, &serialOutput);
            });

    lineSource.EmitLine("first  one   two  three");
}

TEST_F(CommandLineTest, TrailingSpacesDoNotCreateEmptyArguments)
{
    ASSERT_TRUE(commandLine.RegisterCommand(firstCommand));
    commandLine.Run();

    EXPECT_CALL(firstCommand, Execute(testing::_, testing::_))
        .WillOnce([](infra::MemoryRange<const std::string_view> arguments, ISerialOutput&)
            {
                EXPECT_THAT(ToVector(arguments), testing::ElementsAre("one", "two"));
            });

    lineSource.EmitLine("first one two   ");
}

TEST_F(CommandLineTest, CommandWithOnlyTrailingSpacesExecutesWithNoArguments)
{
    ASSERT_TRUE(commandLine.RegisterCommand(firstCommand));
    commandLine.Run();

    EXPECT_CALL(firstCommand, Execute(testing::_, testing::_))
        .WillOnce([](infra::MemoryRange<const std::string_view> arguments, ISerialOutput&)
            {
                EXPECT_TRUE(arguments.empty());
            });

    lineSource.EmitLine("first    ");
}

TEST_F(CommandLineTest, TooManyArgumentsWritesErrorAndDoesNotExecute)
{
    ASSERT_TRUE(commandLine.RegisterCommand(firstCommand));
    commandLine.Run();

    EXPECT_CALL(firstCommand, Execute(testing::_, testing::_)).Times(0);
    EXPECT_CALL(serialOutput, Write(std::string_view("Too many arguments"))).WillOnce(testing::Return(true));
    EXPECT_CALL(serialOutput, Write(testing::A<uint8_t>())).Times(0);

    lineSource.EmitLine("first one two three four");
}
