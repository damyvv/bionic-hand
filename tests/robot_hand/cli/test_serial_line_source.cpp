#include "../../../src/robot_hand/cli/SerialLineSource.hpp"

#include "../test_doubles/SerialInputStub.hpp"

#include <gtest/gtest.h>

#include <array>
#include <string>
#include <vector>

namespace
{
    class SerialLineSourceTest
        : public testing::Test
    {
    protected:
        SerialLineSourceTest()
            : lineSource(serialInput)
        {}

        template<std::size_t Size>
        void Receive(const std::array<uint8_t, Size>& data)
        {
            ASSERT_TRUE(serialInput.HasReceiveHandler());
            serialInput.EmitBytes(data);
        }

        void RegisterLineHandler()
        {
            lineSource.ReceiveLine([this](std::string_view line)
                {
                    receivedLines.emplace_back(line);
                });
        }

        SerialInputStub serialInput;
        SerialLineSource<8> lineSource;
        std::vector<std::string> receivedLines;
    };
}

TEST_F(SerialLineSourceTest, ReceiveLineAccumulatesDataUntilLineFeed)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 3>{ 'a', 'b', 'c' });
    EXPECT_TRUE(receivedLines.empty());

    Receive(std::array<uint8_t, 1>{ '\n' });

    EXPECT_EQ(std::vector<std::string>({ "abc" }), receivedLines);
}

TEST_F(SerialLineSourceTest, ReceiveLineTreatsCarriageReturnAsLineTerminator)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 3>{ 'o', 'k', '\r' });

    EXPECT_EQ(std::vector<std::string>({ "ok" }), receivedLines);
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

    EXPECT_TRUE(firstHandlerLines.empty());
    EXPECT_EQ(std::vector<std::string>({ "x" }), secondHandlerLines);
}

TEST_F(SerialLineSourceTest, MultipleLinesInSingleChunkAreDeliveredInOrder)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 6>{ 'a', '\n', 'b', 'c', '\n', 'd' });

    EXPECT_EQ(std::vector<std::string>({ "a", "bc" }), receivedLines);
}

TEST_F(SerialLineSourceTest, MultipleChunksCanCompleteMultipleLines)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 2>{ 'a', 'b' });
    Receive(std::array<uint8_t, 4>{ '\n', 'c', 'd', '\n' });

    EXPECT_EQ(std::vector<std::string>({ "ab", "cd" }), receivedLines);
}

TEST(SerialLineSourceOverflowTest, OverflowTruncatesLineToBufferCapacity)
{
    SerialInputStub serialInput;
    SerialLineSource<4> lineSource(serialInput);
    std::vector<std::string> receivedLines;

    lineSource.ReceiveLine([&receivedLines](std::string_view line)
        {
            receivedLines.emplace_back(line);
        });

    ASSERT_TRUE(serialInput.HasReceiveHandler());
    serialInput.EmitBytes(std::array<uint8_t, 7>{ 'a', 'b', 'c', 'd', 'e', 'f', '\n' });

    EXPECT_EQ(std::vector<std::string>({ "abcd" }), receivedLines);
}

TEST_F(SerialLineSourceTest, CrLfTerminatesSingleLineWithoutExtraEmptyLine)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 4>{ 'o', 'k', '\r', '\n' });

    EXPECT_EQ(std::vector<std::string>({ "ok" }), receivedLines);
}

TEST_F(SerialLineSourceTest, LfCrTerminatesSingleLineWithoutExtraEmptyLine)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 4>{ 'o', 'k', '\n', '\r' });

    EXPECT_EQ(std::vector<std::string>({ "ok" }), receivedLines);
}

TEST_F(SerialLineSourceTest, CrThenLfInSeparateReceivesWithoutDataBetweenTerminatesSingleLine)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 3>{ 'o', 'k', '\r' });

    Receive(std::array<uint8_t, 1>{ '\n' });

    EXPECT_EQ(std::vector<std::string>({ "ok" }), receivedLines);
}

TEST_F(SerialLineSourceTest, LfThenCrInSeparateReceivesWithoutDataBetweenTerminatesSingleLine)
{
    RegisterLineHandler();

    Receive(std::array<uint8_t, 3>{ 'o', 'k', '\n' });

    Receive(std::array<uint8_t, 1>{ '\r' });

    EXPECT_EQ(std::vector<std::string>({ "ok" }), receivedLines);
}
