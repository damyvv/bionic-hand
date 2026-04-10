#include "../../src/robot_hand/drivers/Servo.hpp"
#include "hal/interfaces/test_doubles/PulseWidthModulationMock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class ServoTest
    : public testing::Test
{
protected:
    testing::StrictMock<hal::PulseWidthModulationMock> pwm;
};

TEST_F(ServoTest, ConstructorDoesNotConfigurePwm)
{
    EXPECT_CALL(pwm, SetPulse(testing::_, testing::_)).Times(0);

    Servo servo(pwm);
}

TEST_F(ServoTest, EmptyServoDoesNotCallSetPulse)
{
    Servo servo;

    EXPECT_CALL(pwm, SetPulse(testing::_, testing::_)).Times(0);

    servo.SetAngle(90.0f);
}

TEST_F(ServoTest, SetsMinimumPulseAtZeroDegrees)
{
    Servo servo(pwm);

    EXPECT_CALL(pwm, SetPulse(544u, 20000u));

    servo.SetAngle(0.0f);
}

TEST_F(ServoTest, SetsMidpointPulseAtNinetyDegrees)
{
    Servo servo(pwm);

    EXPECT_CALL(pwm, SetPulse(1472u, 20000u));

    servo.SetAngle(90.0f);
}

TEST_F(ServoTest, SetsMaximumPulseAtOneHundredEightyDegrees)
{
    Servo servo(pwm);

    EXPECT_CALL(pwm, SetPulse(2400u, 20000u));

    servo.SetAngle(180.0f);
}

TEST_F(ServoTest, ClampsAnglesAboveOneHundredEightyDegrees)
{
    Servo servo(pwm);

    EXPECT_CALL(pwm, SetPulse(2400u, 20000u));

    servo.SetAngle(270.0f);
}

TEST_F(ServoTest, ClampsAnglesBelowZeroDegrees)
{
    Servo servo(pwm);

    EXPECT_CALL(pwm, SetPulse(544u, 20000u));

    servo.SetAngle(-15.0f);
}

TEST_F(ServoTest, UsesConfiguredFrequencyToComputePeriod)
{
    Servo servo(pwm, 100);

    EXPECT_CALL(pwm, SetPulse(1008u, 10000u));

    servo.SetAngle(45.0f);
}
