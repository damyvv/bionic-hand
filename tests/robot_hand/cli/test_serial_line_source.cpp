#include "../../../src/robot_hand/cli/SerialLineSource.hpp"

#include "infra/event/test_helper/EventDispatcherFixture.hpp"
#include "../test_doubles/SerialCommunicationStub.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <string>
#include <vector>

namespace
{
    class SerialLineSourceTest
        : public testing::Test
        , public infra::EventDispatcherFixture
    {
    protected:
        SerialLineSourceTest()
            : lineSource(serial)
        {}

        template<std::size_t Size>
        void Receive(const std::array<uint8_t, Size>& data)
        {
            ASSERT_TRUE(serial.HasReceiveHandler());
            serial.EmitData(infra::MakeRange(data));
        }

        void RegisterLineHandler()
        {
            lineSource.ReceiveLine([this](std::string_view line)
                {
                    receivedLines.emplace_back(line);
                });
        }

        void RegisterByteHandler()
        {
            lineSource.ReceiveByte([this](uint8_t byte)
                {
                    receivedBytes.push_back(byte);
                });
        }

        SerialCommunicationStub serial;
        SerialLineSource<8> lineSource;
        std::vector<std::string> receivedLines;
        std::vector<uint8_t> receivedBytes;
    };
}

TEST_F(SerialLineSourceTest, ConstructorRegistersReceiveCallbackOnSerial)
{
    EXPECT_TRUE(serial.HasReceiveHandler());
}

TEST_F(SerialLineSourceTest, ReceiveLineAccumulatesDataUntilLineFeed)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 3>{ 'a', 'b', 'c' });
    ExecuteAllActions();
    EXPECT_TRUE(receivedLines.empty());

    Receive(std::array<uint8_t, 1>{ '\n' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<std::string>({ "abc" }), receivedLines);
}

TEST_F(SerialLineSourceTest, ReceiveLineTreatsCarriageReturnAsLineTerminator)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 3>{ 'o', 'k', '\r' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<std::string>({ "ok" }), receivedLines);
}

TEST_F(SerialLineSourceTest, ReceiveByteForwardsEachReceivedByteInOrder)
{
    RegisterByteHandler();

    Receive(std::array<uint8_t, 4>{ 'A', 'B', '\r', '\n' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<uint8_t>({ 'A', 'B', '\r', '\n' }), receivedBytes);
}

TEST_F(SerialLineSourceTest, ReceiveLineAndReceiveByteCanBeUsedTogether)
{
    RegisterLineHandler();
    RegisterByteHandler();

    Receive(std::array<uint8_t, 3>{ 'g', 'o', '\n' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<std::string>({ "go" }), receivedLines);
    EXPECT_EQ(std::vector<uint8_t>({ 'g', 'o', '\n' }), receivedBytes);
}

TEST_F(SerialLineSourceTest, LatestLineHandlerReplacesPreviousLineHandler)
{
    std::vector<std::string> firstHandlerLines;
    std::vector<std::string> secondHandlerLines;

    lineSource.ReceiveLine([&firstHandlerLines](std::string_view line)
        {
            firstHandlerLines.emplace_back(line);
        });
    lineSource.ReceiveLine([&secondHandlerLines](std::string_view line)
        {
            secondHandlerLines.emplace_back(line);
        });

    Receive(std::array<uint8_t, 2>{ 'x', '\n' });
    ExecuteAllActions();

    EXPECT_TRUE(firstHandlerLines.empty());
    EXPECT_EQ(std::vector<std::string>({ "x" }), secondHandlerLines);
}

TEST_F(SerialLineSourceTest, LatestByteHandlerReplacesPreviousByteHandler)
{
    std::vector<uint8_t> firstHandlerBytes;
    std::vector<uint8_t> secondHandlerBytes;

    lineSource.ReceiveByte([&firstHandlerBytes](uint8_t byte)
        {
            firstHandlerBytes.push_back(byte);
        });
    lineSource.ReceiveByte([&secondHandlerBytes](uint8_t byte)
        {
            secondHandlerBytes.push_back(byte);
        });

    Receive(std::array<uint8_t, 2>{ 'y', '\n' });
    ExecuteAllActions();

    EXPECT_TRUE(firstHandlerBytes.empty());
    EXPECT_EQ(std::vector<uint8_t>({ 'y', '\n' }), secondHandlerBytes);
}

TEST_F(SerialLineSourceTest, MultipleLinesInSingleChunkAreDeliveredInOrder)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 6>{ 'a', '\n', 'b', 'c', '\n', 'd' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<std::string>({ "a", "bc" }), receivedLines);
}

TEST_F(SerialLineSourceTest, MultipleChunksCanCompleteMultipleLines)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 2>{ 'a', 'b' });
    Receive(std::array<uint8_t, 4>{ '\n', 'c', 'd', '\n' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<std::string>({ "ab", "cd" }), receivedLines);
}

TEST_F(SerialLineSourceTest, EmptyReceiveDoesNotInvokeHandlers)
{
    RegisterLineHandler();
    RegisterByteHandler();

    Receive(std::array<uint8_t, 0>{});
    ExecuteAllActions();

    EXPECT_TRUE(receivedLines.empty());
    EXPECT_TRUE(receivedBytes.empty());
}

TEST(SerialLineSourceOverflowTest, OverflowTruncatesLineToBufferCapacity)
{
    infra::EventDispatcherFixture dispatcher;
    SerialCommunicationStub serial;
    SerialLineSource<4> lineSource(serial);
    std::vector<std::string> receivedLines;
    std::vector<uint8_t> receivedBytes;

    lineSource.ReceiveLine([&receivedLines](std::string_view line)
        {
            receivedLines.emplace_back(line);
        });
    lineSource.ReceiveByte([&receivedBytes](uint8_t byte)
        {
            receivedBytes.push_back(byte);
        });

    ASSERT_TRUE(serial.HasReceiveHandler());
    serial.EmitData(infra::MakeRange(std::array<uint8_t, 7>{ 'a', 'b', 'c', 'd', 'e', 'f', '\n' }));
    dispatcher.ExecuteAllActions();

    EXPECT_EQ(std::vector<std::string>({ "abcd" }), receivedLines);
    EXPECT_EQ(std::vector<uint8_t>({ 'a', 'b', 'c', 'd' }), receivedBytes);
}

TEST_F(SerialLineSourceTest, CrLfTerminatesSingleLineWithoutExtraEmptyLine)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 4>{ 'o', 'k', '\r', '\n' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<std::string>({ "ok" }), receivedLines);
}

TEST_F(SerialLineSourceTest, LfCrTerminatesSingleLineWithoutExtraEmptyLine)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 4>{ 'o', 'k', '\n', '\r' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<std::string>({ "ok" }), receivedLines);
}

TEST_F(SerialLineSourceTest, CrThenLfInSeparateReceivesWithoutDataBetweenTerminatesSingleLine)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 3>{ 'o', 'k', '\r' });
    ExecuteAllActions();

    Receive(std::array<uint8_t, 1>{ '\n' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<std::string>({ "ok" }), receivedLines);
}

TEST_F(SerialLineSourceTest, LfThenCrInSeparateReceivesWithoutDataBetweenTerminatesSingleLine)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 3>{ 'o', 'k', '\n' });
    ExecuteAllActions();

    Receive(std::array<uint8_t, 1>{ '\r' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<std::string>({ "ok" }), receivedLines);
}
