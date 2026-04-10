#include "../../src/robot_hand/Finger.hpp"
#include "test_doubles/ServoMock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{
    class FingerTest
        : public testing::Test
    {
    protected:
        testing::StrictMock<ServoMock> servo;
    };
}

TEST_F(FingerTest, ConstructorDoesNotActuateServo)
{
    EXPECT_CALL(servo, SetAngle(testing::_)).Times(0);

    Finger finger(servo, 90.0f, 30.0f);
}

TEST_F(FingerTest, OpenInterpolatesBetweenClosedAndOpenAngles)
{
    Finger finger(servo, 90.0f, 30.0f);

    EXPECT_CALL(servo, SetAngle(testing::FloatEq(60.0f)));

    finger.Open(0.5f);
}

TEST_F(FingerTest, OpenClampsPercentageBelowZeroToClosedAngle)
{
    Finger finger(servo, 90.0f, 30.0f);

    EXPECT_CALL(servo, SetAngle(testing::FloatEq(30.0f)));

    finger.Open(-1.0f);
}

TEST_F(FingerTest, OpenClampsPercentageAboveOneToOpenAngle)
{
    Finger finger(servo, 90.0f, 30.0f);

    EXPECT_CALL(servo, SetAngle(testing::FloatEq(90.0f)));

    finger.Open(2.0f);
}

TEST_F(FingerTest, OpenWithoutPercentageUsesOpenAngle)
{
    Finger finger(servo, 90.0f, 30.0f);

    EXPECT_CALL(servo, SetAngle(testing::FloatEq(90.0f)));

    finger.Open();
}

TEST_F(FingerTest, CloseUsesClosedAngle)
{
    Finger finger(servo, 90.0f, 30.0f);

    EXPECT_CALL(servo, SetAngle(testing::FloatEq(30.0f)));

    finger.Close();
}
