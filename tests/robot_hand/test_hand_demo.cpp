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

#include "infra/timer/TimerService.hpp"

namespace
{
    class DummyTimerService
        : public infra::TimerService
    {
    public:
        DummyTimerService()
            : infra::TimerService(infra::systemTimerServiceId)
        {}

        void NextTriggerChanged() override
        {}

        infra::TimePoint Now() const override
        {
            return now;
        }

        infra::Duration Resolution() const override
        {
            return std::chrono::milliseconds(1);
        }

    private:
        infra::TimePoint now{};
    };

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

        DummyTimerService timerService;

        testing::NiceMock<HandMock> hand;
        HandDemo demo;
    };

    class PassiveState
        : public DemoState
    {
    public:
        DemoState* Update(HandDemo&) override
        {
            return nullptr;
        }
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

    TEST_F(HandDemoFSMTest, StartDemoTransitionsToOpeningFingersState)
    {
        EXPECT_EQ(demo.currentState, &IdleState::GetInstance());

        demo.StartDemo();

        EXPECT_EQ(demo.currentState, &OpeningFingersState::GetInstance());
        EXPECT_TRUE(demo.timer.has_value());
    }

    TEST_F(HandDemoFSMTest, StopDemoTransitionsToIdleAndClosesFingers)
    {
        demo.StartDemo();

        EXPECT_CALL(hand, CloseFingers());

        demo.StopDemo();

        EXPECT_EQ(demo.currentState, &IdleState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, CancelTimerWithoutActiveTimerIsSafe)
    {
        EXPECT_FALSE(demo.timer.has_value());

        demo.CancelTimer();

        EXPECT_FALSE(demo.timer.has_value());
    }

    TEST_F(HandDemoFSMTest, OpeningStateUpdateTransitionsToClosingAndOpensFingers)
    {
        demo.currentState = &OpeningFingersState::GetInstance();

        EXPECT_CALL(hand, OpenFingers());

        demo.RunFSM();

        EXPECT_EQ(demo.currentState, &ClosingFingersState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, ClosingStateUpdateTransitionsToCountFingersAndClosesFingers)
    {
        demo.currentState = &ClosingFingersState::GetInstance();

        EXPECT_CALL(hand, CloseFingers());

        demo.RunFSM();

        EXPECT_EQ(demo.currentState, &CountFingersState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, CountFingersTransitionsToCountBinaryAfterFingerCountUpdates)
    {
        demo.currentState = &CountFingersState::GetInstance();
        CountFingersState::GetInstance().OnEntry(demo);
        ON_CALL(hand, GetFingerCount()).WillByDefault(testing::Return(5));

        {
            testing::InSequence sequence;
            EXPECT_CALL(hand, OpenFinger(1)).WillOnce(testing::Return(true));
            EXPECT_CALL(hand, OpenFinger(2)).WillOnce(testing::Return(true));
            EXPECT_CALL(hand, OpenFinger(3)).WillOnce(testing::Return(true));
            EXPECT_CALL(hand, OpenFinger(4)).WillOnce(testing::Return(true));
            EXPECT_CALL(hand, OpenFinger(0)).WillOnce(testing::Return(true));
        }

        for (int i = 0; i < 4; ++i)
        {
            demo.RunFSM();
            EXPECT_EQ(demo.currentState, &CountFingersState::GetInstance());
        }

        demo.RunFSM();
        EXPECT_EQ(demo.currentState, &CountBinaryState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, CountBinaryTransitionsToSlowOpenAfterAllCombinations)
    {
        demo.currentState = &CountBinaryState::GetInstance();
        CountBinaryState::GetInstance().OnEntry(demo);
        ON_CALL(hand, GetFingerCount()).WillByDefault(testing::Return(5));
        EXPECT_CALL(hand, OpenFinger(testing::_)).Times(testing::AtLeast(1));
        EXPECT_CALL(hand, CloseFinger(testing::_)).Times(testing::AtLeast(1));

        for (int i = 0; i < (1 << 5); ++i)
            demo.RunFSM();

        EXPECT_EQ(demo.currentState, &CountBinaryState::GetInstance());

        demo.RunFSM();

        EXPECT_EQ(demo.currentState, &SlowOpenState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, SlowOpenTransitionsToSlowCloseAtResolutionBoundary)
    {
        demo.currentState = &SlowOpenState::GetInstance();
        SlowOpenState::GetInstance().OnEntry(demo);
        EXPECT_CALL(hand, OpenFingers(testing::_)).Times(500);

        for (int i = 0; i < 499; ++i)
        {
            demo.RunFSM();
            EXPECT_EQ(demo.currentState, &SlowOpenState::GetInstance());
        }

        demo.RunFSM();
        EXPECT_EQ(demo.currentState, &SlowCloseState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, SlowCloseTransitionsToIdleAtResolutionBoundary)
    {
        demo.currentState = &SlowCloseState::GetInstance();
        SlowCloseState::GetInstance().OnEntry(demo);
        EXPECT_CALL(hand, OpenFingers(testing::_)).Times(500);
        EXPECT_CALL(hand, CloseFingers());

        for (int i = 0; i < 499; ++i)
        {
            demo.RunFSM();
            EXPECT_EQ(demo.currentState, &SlowCloseState::GetInstance());
        }

        demo.RunFSM();
        EXPECT_EQ(demo.currentState, &IdleState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, IdleUpdateReturnsNullAndKeepsIdleState)
    {
        demo.currentState = &IdleState::GetInstance();

        demo.RunFSM();

        EXPECT_EQ(demo.currentState, &IdleState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, DemoStateDefaultHooksCanBeCalled)
    {
        PassiveState passive;

        passive.OnEntry(demo);
        passive.OnExit(demo);

        EXPECT_EQ(passive.Update(demo), nullptr);
    }
}
