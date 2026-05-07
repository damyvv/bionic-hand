#include "../../../src/robot_hand/cli/SerialOutput.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <deque>
#include <vector>

namespace
{
    class SerialCommunicationDouble
        : public hal::SerialCommunication
    {
    public:
        MOCK_METHOD(void, SendData, (infra::ConstByteRange data, infra::Function<void()> actionOnCompletion), (override));
        MOCK_METHOD(void, ReceiveData, (infra::Function<void(infra::ConstByteRange data)> dataReceived), (override));
    };

    class SerialOutputTest
        : public testing::Test
    {
    protected:
        void ExpectNextSend(const std::vector<uint8_t>& expectedBytes)
        {
            EXPECT_CALL(serial, SendData(testing::_, testing::_))
                .WillOnce([this, expectedBytes](infra::ConstByteRange data, infra::Function<void()> onDone)
                    {
                        EXPECT_EQ(expectedBytes, std::vector<uint8_t>(data.begin(), data.end()));
                        completionCallbacks.push_back(std::move(onDone));
                    });
        }

        void CompleteNextSend()
        {
            ASSERT_FALSE(completionCallbacks.empty());
            auto callback = std::move(completionCallbacks.front());
            completionCallbacks.pop_front();
            callback();
        }

        testing::StrictMock<SerialCommunicationDouble> serial;
        SerialOutput<4> serialOutput{ serial };
        std::deque<infra::Function<void()>> completionCallbacks;
    };
}

TEST_F(SerialOutputTest, WriteStringWhenIdleSendsImmediately)
{
    ExpectNextSend({ 'O', 'K' });

    EXPECT_TRUE(serialOutput.Write("OK"));
    ASSERT_EQ(1u, completionCallbacks.size());
}

TEST_F(SerialOutputTest, WriteByteWhenIdleSendsImmediately)
{
    ExpectNextSend({ 0xAB });

    EXPECT_TRUE(serialOutput.Write(static_cast<uint8_t>(0xAB)));
    ASSERT_EQ(1u, completionCallbacks.size());
}

TEST_F(SerialOutputTest, WriteWhileBusyQueuesDataUntilCompletion)
{
    testing::InSequence sequence;

    ExpectNextSend({ 'a', 'b' });
    ASSERT_TRUE(serialOutput.Write("ab"));

    EXPECT_TRUE(serialOutput.Write("cd"));

    ExpectNextSend({ 'c', 'd' });
    CompleteNextSend();

    ASSERT_EQ(1u, completionCallbacks.size());
    CompleteNextSend();
    EXPECT_TRUE(completionCallbacks.empty());
}

TEST_F(SerialOutputTest, WriteStringReturnsFalseWhenThereIsNoQueueSpace)
{
    ExpectNextSend({ 'a', 'b', 'c' });
    ASSERT_TRUE(serialOutput.Write("abc"));

    EXPECT_FALSE(serialOutput.Write("de"));

    ASSERT_EQ(1u, completionCallbacks.size());
    CompleteNextSend();
    EXPECT_TRUE(completionCallbacks.empty());
}

TEST(SerialOutputSmallBufferTest, WriteByteReturnsFalseWhenQueueIsFull)
{
    testing::StrictMock<SerialCommunicationDouble> serial;
    SerialOutput<4> serialOutput{ serial };
    std::deque<infra::Function<void()>> completionCallbacks;

    EXPECT_CALL(serial, SendData(testing::_, testing::_))
        .WillOnce([&completionCallbacks](infra::ConstByteRange data, infra::Function<void()> onDone)
            {
                EXPECT_EQ(std::vector<uint8_t>({ 'w' }), std::vector<uint8_t>(data.begin(), data.end()));
                completionCallbacks.push_back(std::move(onDone));
            });

    EXPECT_TRUE(serialOutput.Write(static_cast<uint8_t>('w')));
    EXPECT_TRUE(serialOutput.Write(static_cast<uint8_t>('x')));
    EXPECT_TRUE(serialOutput.Write(static_cast<uint8_t>('y')));
    EXPECT_TRUE(serialOutput.Write(static_cast<uint8_t>('z')));
    EXPECT_FALSE(serialOutput.Write(static_cast<uint8_t>('!')));
}

TEST_F(SerialOutputTest, EmptyStringWriteSucceedsWithoutSending)
{
    EXPECT_CALL(serial, SendData(testing::_, testing::_)).Times(0);

    EXPECT_TRUE(serialOutput.Write(""));
    EXPECT_TRUE(completionCallbacks.empty());
}
