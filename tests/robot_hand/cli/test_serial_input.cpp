#include "../../../src/robot_hand/cli/SerialInput.hpp"

#include "infra/event/test_helper/EventDispatcherFixture.hpp"
#include "../test_doubles/SerialCommunicationStub.hpp"

#include <gtest/gtest.h>

#include <array>
#include <vector>

namespace
{
    class SerialInputTest
        : public testing::Test
        , public infra::EventDispatcherFixture
    {
    protected:
        SerialInputTest()
            : serialInput(serial)
        {}

        template<std::size_t Size>
        void Receive(const std::array<uint8_t, Size>& data)
        {
            ASSERT_TRUE(serial.HasReceiveHandler());
            serial.EmitData(infra::MakeRange(data));
        }

        void RegisterByteHandler()
        {
            serialInput.ReceiveByte([this](uint8_t byte)
                {
                    receivedBytes.push_back(byte);
                });
        }

        SerialCommunicationStub serial;
        SerialInput<8> serialInput;
        std::vector<uint8_t> receivedBytes;
    };
}

TEST_F(SerialInputTest, ConstructorRegistersReceiveCallbackOnSerial)
{
    EXPECT_TRUE(serial.HasReceiveHandler());
}

TEST_F(SerialInputTest, ReceiveByteForwardsEachReceivedByteInOrder)
{
    RegisterByteHandler();

    Receive(std::array<uint8_t, 4>{ 'A', 'B', '\r', '\n' });
    ExecuteAllActions();

    EXPECT_EQ(std::vector<uint8_t>({ 'A', 'B', '\r', '\n' }), receivedBytes);
}

TEST_F(SerialInputTest, LatestByteHandlerReplacesPreviousByteHandler)
{
    std::vector<uint8_t> firstHandlerBytes;
    std::vector<uint8_t> secondHandlerBytes;

    serialInput.ReceiveByte([&firstHandlerBytes](uint8_t byte)
        {
            firstHandlerBytes.push_back(byte);
        });
    serialInput.ReceiveByte([&secondHandlerBytes](uint8_t byte)
        {
            secondHandlerBytes.push_back(byte);
        });

    Receive(std::array<uint8_t, 2>{ 'y', '\n' });
    ExecuteAllActions();

    EXPECT_TRUE(firstHandlerBytes.empty());
    EXPECT_EQ(std::vector<uint8_t>({ 'y', '\n' }), secondHandlerBytes);
}

TEST_F(SerialInputTest, EmptyReceiveDoesNotInvokeHandlers)
{
    RegisterByteHandler();

    Receive(std::array<uint8_t, 0>{});
    ExecuteAllActions();

    EXPECT_TRUE(receivedBytes.empty());
}
