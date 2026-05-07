#include "hal/interfaces/test_doubles/I2cMock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../src/robot_hand/drivers/Pca9685.hpp"

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

TEST_F(Pca9685Test, ConstructorSetsErrorPolicyAndInitializesDevice)
{
    testing::InSequence sequence;
    hal::I2cErrorPolicyMock errorPolicy;
    EXPECT_CALL(i2c, SetErrorPolicy(testing::Ref(errorPolicy)));
    ExpectInitialization();

    Pca9685 pca9685(i2c, kAddress, errorPolicy);

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

TEST_F(Pca9685QueueTest, SetFrequencyI2CQueueIsFull)
{
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x00, 0x20 }));

    Pca9685 pca9685(i2c, kAddress);

    while (!pca9685.IsMessageQueueFull()) {
        pca9685.SetChannelPulseOn(0, 0x01FF);
    }

    EXPECT_TRUE(pca9685.IsMessageQueueFull());
    EXPECT_DEATH(pca9685.SetFrequency(100), "");
}

TEST_F(Pca9685QueueTest, SetChannelPulseOnQueueIsFull)
{
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x00, 0x20 }));

    Pca9685 pca9685(i2c, kAddress);

    while (!pca9685.IsMessageQueueFull()) {
        pca9685.SetChannelPulseOn(0, 0x01FF);
    }

    EXPECT_TRUE(pca9685.IsMessageQueueFull());
    EXPECT_DEATH(pca9685.SetChannelPulseOn(1, 0x01FF), "");
}

TEST_F(Pca9685Test, GetMessageQueueFullReturnsFalseWhenQueueIsNotFull)
{
    testing::InSequence sequence;
    ExpectInitialization();

    Pca9685 pca9685(i2c, kAddress);

    EXPECT_FALSE(pca9685.IsMessageQueueFull());
}

TEST_F(Pca9685QueueTest, GetMessageQueueFullReturnsTrueWhenQueueIsFull)
{
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x00, 0x20 }));
    
    Pca9685 pca9685(i2c, kAddress);

    while (!pca9685.IsMessageQueueFull()) {
        pca9685.SetChannelPulseOn(0, 0x01FF);
    }

    EXPECT_TRUE(pca9685.IsMessageQueueFull());
}

TEST_F(Pca9685Test, PrescaleBoundaryValues)
{
    testing::InSequence sequence;
    ExpectInitialization();
    
    Pca9685 pca9685(i2c, kAddress);
    
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0xFE, 0x01 }));
    pca9685.SetFrequency(3052);
    
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0xFE, 0x00 }));
    pca9685.SetFrequency(6104);
    
    EXPECT_EQ(6104u, pca9685.GetFrequency());
    EXPECT_EQ(164u, pca9685.GetPeriodInMicroseconds());

    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0xFE, 0xFD }));
    pca9685.SetFrequency(24);
    
    EXPECT_EQ(24u, pca9685.GetFrequency());
    EXPECT_EQ(41667u, pca9685.GetPeriodInMicroseconds());
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
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x12, 0x00, 0x00, 0xFF, 0x01 }));

    Pca9685 pca9685(i2c, kAddress);
    bool callbackInvoked = false;

    pca9685.SetChannelPulseOn(3, 0x01FF, [&callbackInvoked](hal::Result result)
        {
            callbackInvoked = (result == hal::Result::complete);
        });

    EXPECT_TRUE(callbackInvoked);
}

TEST_F(Pca9685Test, ErrorPolicyIsAppliedToI2cMaster)
{
    testing::InSequence sequence;
    ExpectInitialization();
    hal::I2cErrorPolicyMock errorPolicy;

    Pca9685 pca9685(i2c, kAddress);

    EXPECT_CALL(i2c, SetErrorPolicy(testing::Ref(errorPolicy)));
    pca9685.SetErrorPolicy(errorPolicy);
}

TEST_F(Pca9685Test, ResetErrorPolicyIsAppliedToI2cMaster)
{
    testing::InSequence sequence;
    ExpectInitialization();
    hal::I2cErrorPolicyMock errorPolicy;

    Pca9685 pca9685(i2c, kAddress);

    EXPECT_CALL(i2c, ResetErrorPolicy());
    pca9685.ResetErrorPolicy();
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
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x0E, 0x00, 0x00, 0x00, 0x04 }));

    Pca9685 pca9685(i2c, kAddress);
    pca9685.GetChannel(2).SetDuty(25);
}

TEST_F(Pca9685Test, ChannelSetDutyClampsValuesAboveOneHundredPercent)
{
    testing::InSequence sequence;
    ExpectInitialization();
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x0A, 0x00, 0x00, 0x00, 0x10 }));

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
    EXPECT_CALL(i2c, SendDataMock(kAddress, hal::Action::stop, std::vector<uint8_t>{ 0x06, 0x00, 0x00, 0x99, 0x01 }));
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
