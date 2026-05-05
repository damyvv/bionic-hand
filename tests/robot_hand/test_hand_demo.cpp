#ifndef FINGER_COUNT
#define FINGER_COUNT 5
#endif

#define private public
#include "../../src/robot_hand/demo/HandDemo.hpp"
#undef private

#include "../../src/robot_hand/demo/DemoStates.hpp"
#include "../../src/robot_hand/Hand.hpp"

#include "test_doubles/HandMock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{
    class MockDemoState
        : public DemoState
    {
    public:
        MOCK_METHOD(void, OnEntry, (HandDemo& demo), (override));
        MOCK_METHOD(void, OnExit, (HandDemo& demo), (override));
        MOCK_METHOD(DemoState*, Update, (HandDemo& demo), (override));
    };

    class HandDemoFSMTest
        : public testing::Test
    {
    protected:
        HandDemoFSMTest()
            : demo(hand)
        {}

        testing::NiceMock<HandMock> hand;
        HandDemo demo;
    };

    TEST_F(HandDemoFSMTest, ConstructorInitializesIdleState)
    {
        EXPECT_EQ(demo.currentState, &IdleState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, GetHandReturnsOriginalReference)
    {
        EXPECT_EQ(&demo.GetHand(), &hand);
    }

    TEST_F(HandDemoFSMTest, TransitionToStateCallsExitThenEntry)
    {
        testing::StrictMock<MockDemoState> fromState;
        testing::StrictMock<MockDemoState> toState;

        demo.currentState = &fromState;

        testing::InSequence s;
        EXPECT_CALL(fromState, OnExit(testing::Ref(demo)));
        EXPECT_CALL(toState, OnEntry(testing::Ref(demo)));

        demo.TransitionToState(&toState);

        EXPECT_EQ(demo.currentState, &toState);
    }

    TEST_F(HandDemoFSMTest, RunFSMNoTransitionWhenUpdateReturnsNull)
    {
        testing::StrictMock<MockDemoState> current;
        demo.currentState = &current;

        EXPECT_CALL(current, Update(testing::Ref(demo))).WillOnce(testing::Return(nullptr));
        EXPECT_CALL(current, OnExit(testing::_)).Times(0);

        demo.RunFSM();

        EXPECT_EQ(demo.currentState, &current);
    }

    TEST_F(HandDemoFSMTest, RunFSMTransitionsWhenUpdateReturnsNextState)
    {
        testing::StrictMock<MockDemoState> current;
        testing::StrictMock<MockDemoState> next;
        demo.currentState = &current;

        testing::InSequence s;
        EXPECT_CALL(current, Update(testing::Ref(demo))).WillOnce(testing::Return(&next));
        EXPECT_CALL(current, OnExit(testing::Ref(demo)));
        EXPECT_CALL(next, OnEntry(testing::Ref(demo)));

        demo.RunFSM();

        EXPECT_EQ(demo.currentState, &next);
    }

    TEST_F(HandDemoFSMTest, TransitionToStateWorksWhenCurrentIsNull)
    {
        testing::StrictMock<MockDemoState> next;
        demo.currentState = nullptr;

        EXPECT_CALL(next, OnEntry(testing::Ref(demo)));

        demo.TransitionToState(&next);

        EXPECT_EQ(demo.currentState, &next);
    }
}
