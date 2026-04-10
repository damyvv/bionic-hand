#include "hal/interfaces/test_doubles/I2cMock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "../../src/robot_hand/drivers/Pca9685.hpp"
#undef private

namespace
{
    const hal::I2cAddress kAddress{ 0x40 };

    class Pca9685Test
        : public testing::Test
    {
    protected:
        void ExpectInitialization(uint8_t prescale = 0x79)
        {
            EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x00, 0x20 }));
            EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0xFE, prescale }));
        }

        testing::StrictMock<hal::I2cMasterMock> i2c;
    };

    class Pca9685QueueTest
        : public testing::Test
    {
    protected:
        testing::StrictMock<hal::I2cMasterMockWithoutAutomaticDone> i2c;
    };
}

TEST_F(Pca9685Test, ConstructorInitializesDeviceAndSetsDefaultFrequency)
{
    testing::InSequence sequence;
    ExpectInitialization();

    Pca9685 pca9685(i2c, kAddress);

    EXPECT_EQ(50, pca9685.GetFrequency());
    EXPECT_EQ(20000u, pca9685.GetPeriodInMicroseconds());
}

TEST_F(Pca9685Test, ConstructorInitializesDeviceAndSetsFrequency)
{
    testing::InSequence sequence;
    ExpectInitialization(0x1E);

    Pca9685 pca9685(i2c, kAddress, 200);

    EXPECT_EQ(200, pca9685.GetFrequency());
    EXPECT_EQ(5000u, pca9685.GetPeriodInMicroseconds());
}

TEST_F(Pca9685Test, SetFrequencyUpdatesPrescaleAndCachedPeriod)
{
    testing::InSequence sequence;
    ExpectInitialization();
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0xFE, 0x3C }));

    Pca9685 pca9685(i2c, kAddress);
    pca9685.SetFrequency(100);

    EXPECT_EQ(100, pca9685.GetFrequency());
    EXPECT_EQ(10000u, pca9685.GetPeriodInMicroseconds());
}

TEST_F(Pca9685Test, SetChannelPulseOnWritesExpectedRegisters)
{
    testing::InSequence sequence;
    ExpectInitialization();
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x12, 0x00, 0x00, 0xFF, 0x01 }));

    Pca9685 pca9685(i2c, kAddress);
    pca9685.SetChannelPulseOn(3, 0x01FF);
}

TEST_F(Pca9685Test, InvokesQueuedMessageCallbackWhenPresent)
{
    testing::InSequence sequence;
    ExpectInitialization();
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0xAA, 0x55 }));

    Pca9685 pca9685(i2c, kAddress);
    bool callbackInvoked = false;

    pca9685.i2cMessageQueue.push(Pca9685Message{ std::vector<uint8_t>{ 0xAA, 0x55 }, [&callbackInvoked](hal::Result result)
        {
            callbackInvoked = (result == hal::Result::complete);
        } });

    pca9685.ProcessI2cMessageQueue();

    EXPECT_TRUE(callbackInvoked);
}

TEST_F(Pca9685QueueTest, QueuesAdditionalWritesWhileBusIsBusy)
{
    testing::InSequence sequence;
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x00, 0x20 }));
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0xFE, 0x79 }));
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0xFE, 0x3C }));

    Pca9685 pca9685(i2c, kAddress);
    pca9685.SetFrequency(100);

    ASSERT_TRUE(static_cast<bool>(i2c.onSent));
    i2c.onSent(hal::Result::complete, 2);

    ASSERT_TRUE(static_cast<bool>(i2c.onSent));
    i2c.onSent(hal::Result::complete, 2);

    i2c.onSent(hal::Result::complete, 2);
}

TEST(Pca9685DeathTest, GetChannelRejectsOutOfRangeChannel)
{
    testing::NiceMock<hal::I2cMasterMock> i2c;
    Pca9685 pca9685(i2c, kAddress);

    EXPECT_DEATH(static_cast<void>(pca9685.GetChannel(16)), "");
}

TEST_F(Pca9685Test, ChannelSetDutyConvertsDutyCycleToPulseWidth)
{
    testing::InSequence sequence;
    ExpectInitialization();
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x0E, 0x00, 0x00, 0x88, 0x13 }));

    Pca9685 pca9685(i2c, kAddress);
    pca9685.GetChannel(2).SetDuty(25);
}

TEST_F(Pca9685Test, ChannelSetDutyClampsValuesAboveOneHundredPercent)
{
    testing::InSequence sequence;
    ExpectInitialization();
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x0A, 0x00, 0x00, 0x20, 0x4E }));

    Pca9685 pca9685(i2c, kAddress);
    pca9685.GetChannel(1).SetDuty(150);
}

TEST_F(Pca9685Test, ChannelSetPulseWritesRequestedPulseWhenPeriodMatches)
{
    testing::InSequence sequence;
    ExpectInitialization();
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x16, 0x00, 0x00, 0xDC, 0x05 }));

    Pca9685 pca9685(i2c, kAddress);
    pca9685.GetChannel(4).SetPulse(1500, 20000);
}

TEST_F(Pca9685Test, ChannelStartReappliesTheLastPulseWidth)
{
    testing::InSequence sequence;
    ExpectInitialization();
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x12, 0x00, 0x00, 0xE8, 0x03 }));
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x12, 0x00, 0x00, 0xE8, 0x03 }));

    Pca9685 pca9685(i2c, kAddress);
    Pca9685Channel& channel = pca9685.GetChannel(3);

    channel.SetPulse(1000, 20000);
    channel.Start();
}

TEST_F(Pca9685Test, ChannelStopWritesZeroPulseWidth)
{
    testing::InSequence sequence;
    ExpectInitialization();
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x06, 0x00, 0x00, 0xD0, 0x07 }));
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x06, 0x00, 0x00, 0x00, 0x00 }));

    Pca9685 pca9685(i2c, kAddress);
    Pca9685Channel& channel = pca9685.GetChannel(0);

    channel.SetDuty(10);
    channel.Stop();
}

TEST(Pca9685ChannelDeathTest, SetPulseRejectsMismatchedPeriod)
{
    testing::NiceMock<hal::I2cMasterMock> i2c;
    Pca9685 pca9685(i2c, kAddress);

    EXPECT_DEATH(pca9685.GetChannel(0).SetPulse(1000, 12345), "");
}
