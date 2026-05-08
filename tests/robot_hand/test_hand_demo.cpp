#ifndef FINGER_COUNT
#define FINGER_COUNT 5
#endif

#include "../../src/robot_hand/demo/HandDemo.hpp"
#include "../../src/robot_hand/demo/DemoStates.hpp"
#include "../../src/robot_hand/Hand.hpp"

#include "test_doubles/HandMock.hpp"
#include "test_doubles/SerialOutputMock.hpp"

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

        void TriggerNext()
        {
            now = NextTrigger();
            Progressed(now);
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
            : demo(hand, timerService, serialOutputMock)
        {
            demo.SetupTimer(std::chrono::milliseconds(1));
        }

        void SetUp() override
        {
            ResetStateTransitions();
            WaveState::GetInstance().SetWaveCount(2);
        }

        void TearDown() override
        {
            ResetStateTransitions();
        }

        void ResetStateTransitions()
        {
            OpeningFingersState::GetInstance().SetNextState(nullptr);
            ClosingFingersState::GetInstance().SetNextState(nullptr);
            CountFingersState::GetInstance().SetNextState(nullptr);
            WaveState::GetInstance().SetNextState(nullptr);
            SlowOpenState::GetInstance().SetNextState(nullptr);
            SlowCloseState::GetInstance().SetNextState(nullptr);
            IdleState::GetInstance().SetNextState(nullptr);
        }

        DummyTimerService timerService;

        testing::NiceMock<HandMock> hand;
        testing::NiceMock<SerialOutputMock> serialOutputMock;
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
        EXPECT_EQ(demo.GetCurrentState(), &IdleState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, GetHandReturnsOriginalReference)
    {
        EXPECT_EQ(&demo.GetHand(), &hand);
    }

    TEST_F(HandDemoFSMTest, TransitionToStateCallsExitThenEntry)
    {
        testing::StrictMock<MockDemoState> fromState;
        testing::StrictMock<MockDemoState> toState;

        EXPECT_CALL(fromState, OnEntry(testing::Ref(demo)));
        demo.TransitionToState(&fromState);

        testing::InSequence s;
        EXPECT_CALL(fromState, OnExit(testing::Ref(demo)));
        EXPECT_CALL(toState, OnEntry(testing::Ref(demo)));

        demo.TransitionToState(&toState);

        EXPECT_EQ(demo.GetCurrentState(), &toState);
    }

    TEST_F(HandDemoFSMTest, RunFSMNoTransitionWhenUpdateReturnsNull)
    {
        testing::StrictMock<MockDemoState> current;

        EXPECT_CALL(current, OnEntry(testing::Ref(demo)));
        demo.TransitionToState(&current);

        EXPECT_CALL(current, Update(testing::Ref(demo))).WillOnce(testing::Return(nullptr));
        EXPECT_CALL(current, OnExit(testing::_)).Times(0);

        timerService.TriggerNext();

        EXPECT_EQ(demo.GetCurrentState(), &current);
    }

    TEST_F(HandDemoFSMTest, RunFSMTransitionsWhenUpdateReturnsNextState)
    {
        testing::StrictMock<MockDemoState> current;
        testing::StrictMock<MockDemoState> next;

        EXPECT_CALL(current, OnEntry(testing::Ref(demo)));
        demo.TransitionToState(&current);

        testing::InSequence s;
        EXPECT_CALL(current, Update(testing::Ref(demo))).WillOnce(testing::Return(&next));
        EXPECT_CALL(current, OnExit(testing::Ref(demo)));
        EXPECT_CALL(next, OnEntry(testing::Ref(demo)));

        timerService.TriggerNext();

        EXPECT_EQ(demo.GetCurrentState(), &next);
    }

    TEST_F(HandDemoFSMTest, TransitionToStateWorksWhenCurrentIsNull)
    {
        testing::StrictMock<MockDemoState> next;
        EXPECT_CALL(next, OnEntry(testing::Ref(demo)));

        demo.TransitionToState(&next);

        EXPECT_EQ(demo.GetCurrentState(), &next);
    }

    TEST_F(HandDemoFSMTest, StartDemoTransitionsToOpeningFingersState)
    {
        EXPECT_EQ(demo.GetCurrentState(), &IdleState::GetInstance());

        demo.StartDemo();

        EXPECT_EQ(demo.GetCurrentState(), &OpeningFingersState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, StopDemoTransitionsToIdleAndClosesFingers)
    {
        demo.StartDemo();

        EXPECT_CALL(hand, CloseFingers());

        demo.StopDemo();

        EXPECT_EQ(demo.GetCurrentState(), &IdleState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, OpeningStateUpdateTransitionsToConfiguredNextStateAndOpensFingers)
    {
        testing::NiceMock<MockDemoState> nextState;
        OpeningFingersState::GetInstance().SetNextState(&nextState);
        demo.TransitionToState(&OpeningFingersState::GetInstance());

        EXPECT_CALL(hand, OpenFingers());

        timerService.TriggerNext();

        EXPECT_EQ(demo.GetCurrentState(), &nextState);
    }

    TEST_F(HandDemoFSMTest, ClosingStateUpdateTransitionsToConfiguredNextStateAndClosesFingers)
    {
        testing::NiceMock<MockDemoState> nextState;
        ClosingFingersState::GetInstance().SetNextState(&nextState);
        demo.TransitionToState(&ClosingFingersState::GetInstance());

        EXPECT_CALL(hand, CloseFingers());

        timerService.TriggerNext();

        EXPECT_EQ(demo.GetCurrentState(), &nextState);
    }

    TEST_F(HandDemoFSMTest, CountFingersTransitionsToConfiguredNextStateAfterFingerCountUpdates)
    {
        testing::NiceMock<MockDemoState> nextState;
        CountFingersState::GetInstance().SetNextState(&nextState);
        demo.TransitionToState(&CountFingersState::GetInstance());
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

        for (int i = 0; i < 5; ++i)
        {
            timerService.TriggerNext();
            EXPECT_EQ(demo.GetCurrentState(), &CountFingersState::GetInstance());
        }

        timerService.TriggerNext();
        EXPECT_EQ(demo.GetCurrentState(), &CountFingersState::GetInstance());

        timerService.TriggerNext();
        EXPECT_EQ(demo.GetCurrentState(), &nextState);
    }

    TEST_F(HandDemoFSMTest, WaveTransitionsToConfiguredNextStateAfterWaveCountUpdates)
    {
        testing::NiceMock<MockDemoState> nextState;
        WaveState::GetInstance().SetNextState(&nextState);
        WaveState::GetInstance().SetWaveCount(1);
        demo.TransitionToState(&WaveState::GetInstance());
        WaveState::GetInstance().OnEntry(demo);
        ON_CALL(hand, GetFingerCount()).WillByDefault(testing::Return(5));
        EXPECT_CALL(hand, OpenFinger(testing::_, testing::_)).Times(testing::AtLeast(1));

        while (demo.GetCurrentState() == &WaveState::GetInstance())
            timerService.TriggerNext();

        EXPECT_EQ(demo.GetCurrentState(), &nextState);
    }

    TEST_F(HandDemoFSMTest, SlowOpenTransitionsToConfiguredNextStateAtResolutionBoundary)
    {
        testing::NiceMock<MockDemoState> nextState;
        SlowOpenState::GetInstance().SetNextState(&nextState);
        demo.TransitionToState(&SlowOpenState::GetInstance());
        SlowOpenState::GetInstance().OnEntry(demo);
        EXPECT_CALL(hand, OpenFingers(testing::_)).Times(500);

        for (int i = 0; i < 499; ++i)
        {
            timerService.TriggerNext();
            EXPECT_EQ(demo.GetCurrentState(), &SlowOpenState::GetInstance());
        }

        timerService.TriggerNext();
        EXPECT_EQ(demo.GetCurrentState(), &nextState);
    }

    TEST_F(HandDemoFSMTest, SlowCloseTransitionsToConfiguredNextStateAtResolutionBoundary)
    {
        testing::NiceMock<MockDemoState> nextState;
        SlowCloseState::GetInstance().SetNextState(&nextState);
        demo.TransitionToState(&SlowCloseState::GetInstance());
        SlowCloseState::GetInstance().OnEntry(demo);
        EXPECT_CALL(hand, OpenFingers(testing::_)).Times(500);

        for (int i = 0; i < 499; ++i)
        {
            timerService.TriggerNext();
            EXPECT_EQ(demo.GetCurrentState(), &SlowCloseState::GetInstance());
        }

        timerService.TriggerNext();
        EXPECT_EQ(demo.GetCurrentState(), &nextState);
    }

    TEST_F(HandDemoFSMTest, IdleUpdateReturnsNullAndKeepsIdleState)
    {
        demo.TransitionToState(&IdleState::GetInstance());

        timerService.TriggerNext();

        EXPECT_EQ(demo.GetCurrentState(), &IdleState::GetInstance());
    }

    TEST_F(HandDemoFSMTest, DemoStateDefaultHooksCanBeCalled)
    {
        PassiveState passive;

        passive.OnEntry(demo);
        passive.OnExit(demo);

        EXPECT_EQ(passive.Update(demo), nullptr);
    }
}
