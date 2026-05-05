#include "../../src/robot_hand/Hand.hpp"
#include "hal/interfaces/test_doubles/PulseWidthModulationMock.hpp"
#include "test_doubles/ServoMock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{
    class HandTest
        : public testing::Test
    {
    protected:
        Hand<5> CreateHand()
        {
            return Hand<5>({
                std::ref(fingers[0]),
                std::ref(fingers[1]),
                std::ref(fingers[2]),
                std::ref(fingers[3]),
                std::ref(fingers[4])
            });
        }

        std::array<testing::StrictMock<ServoMock>, 5> servos;
        std::array<Finger, 5> fingers{
            Finger(servos[0], 10.0f, 100.0f),
            Finger(servos[1], 20.0f, 110.0f),
            Finger(servos[2], 30.0f, 120.0f),
            Finger(servos[3], 40.0f, 130.0f),
            Finger(servos[4], 50.0f, 140.0f)
        };
    };
}

TEST_F(HandTest, ConstructorDoesNotActuateAnyFinger)
{
    for (auto& servo : servos)
        EXPECT_CALL(servo, SetAngle(testing::_)).Times(0);

    Hand<5> hand = CreateHand();
}

TEST_F(HandTest, GetFingerReturnsRequestedFinger)
{
    Hand<5> hand = CreateHand();

    EXPECT_CALL(servos[2], SetAngle(30.0f));

    auto finger = hand.GetFinger(2);
    ASSERT_TRUE(finger.has_value());
    finger->get().Open();
}

TEST_F(HandTest, OpenFingerWithPercentageTargetsRequestedFinger)
{
    Hand<5> hand = CreateHand();

    EXPECT_CALL(servos[0], SetAngle(55.0f));

    hand.OpenFinger(0, 0.5f);
}

TEST_F(HandTest, OpenFingerWithoutPercentageUsesFingerOpenAngle)
{
    Hand<5> hand = CreateHand();

    EXPECT_CALL(servos[1], SetAngle(20.0f));

    hand.OpenFinger(1);
}

TEST_F(HandTest, CloseFingerTargetsRequestedFinger)
{
    Hand<5> hand = CreateHand();

    EXPECT_CALL(servos[4], SetAngle(140.0f));

    hand.CloseFinger(4);
}

TEST_F(HandTest, OpenFingersWithPercentageOpensAllFingers)
{
    Hand<5> hand = CreateHand();
    testing::InSequence sequence;

    EXPECT_CALL(servos[0], SetAngle(55.0f));
    EXPECT_CALL(servos[1], SetAngle(65.0f));
    EXPECT_CALL(servos[2], SetAngle(75.0f));
    EXPECT_CALL(servos[3], SetAngle(85.0f));
    EXPECT_CALL(servos[4], SetAngle(95.0f));

    hand.OpenFingers(0.5f);
}

TEST_F(HandTest, OpenFingersWithoutPercentageFullyOpensAllFingers)
{
    Hand<5> hand = CreateHand();
    testing::InSequence sequence;

    EXPECT_CALL(servos[0], SetAngle(10.0f));
    EXPECT_CALL(servos[1], SetAngle(20.0f));
    EXPECT_CALL(servos[2], SetAngle(30.0f));
    EXPECT_CALL(servos[3], SetAngle(40.0f));
    EXPECT_CALL(servos[4], SetAngle(50.0f));

    hand.OpenFingers();
}

TEST_F(HandTest, CloseFingersClosesAllFingers)
{
    Hand<5> hand = CreateHand();
    testing::InSequence sequence;

    EXPECT_CALL(servos[0], SetAngle(100.0f));
    EXPECT_CALL(servos[1], SetAngle(110.0f));
    EXPECT_CALL(servos[2], SetAngle(120.0f));
    EXPECT_CALL(servos[3], SetAngle(130.0f));
    EXPECT_CALL(servos[4], SetAngle(140.0f));

    hand.CloseFingers();
}
