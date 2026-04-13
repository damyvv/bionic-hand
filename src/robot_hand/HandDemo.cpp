#include "HandDemo.hpp"
#include "infra/timer/Timer.hpp"

constexpr uint32_t slowMoveResolution = 500; // number of steps to go from fully closed to fully open in slow open/close states
constexpr infra::Duration slowTimerPeriod = std::chrono::milliseconds(350);
constexpr infra::Duration fastTimerPeriod = std::chrono::milliseconds(5);

enum class HandDemoState
{
    OpeningFingers,
    ClosingFingers,
    CountFingers,
    CountBinary,
    SlowOpen,
    SlowClose,
    Idle
};

HandDemo::HandDemo(Hand<FINGER_COUNT>& hand)
    : hand(hand)
    , timer()
    , state(HandDemoState::Idle)
{
}

void HandDemo::StartDemo()
{
    counter = 0;
    state = HandDemoState::OpeningFingers;
    SetupTimer(slowTimerPeriod);
}

void HandDemo::SetupTimer(infra::Duration period)
{
    timer.Cancel();
    timer.Start(period, [this]() { RunFSM(); });
}

void HandDemo::RunFSM()
{

    switch (state)
    {
    case HandDemoState::OpeningFingers:
        hand.OpenFingers();
        state = HandDemoState::ClosingFingers;
        break;
    case HandDemoState::ClosingFingers:
        hand.CloseFingers();
        state = HandDemoState::CountFingers;
        break;
    case HandDemoState::CountFingers:
        hand.OpenFinger((counter + 1) % FINGER_COUNT);
        ++counter;
        if (counter == FINGER_COUNT) {
            state = HandDemoState::CountBinary;
            counter = 0;
        }
        break;
    case HandDemoState::CountBinary:
        if (counter == (1 << FINGER_COUNT)) {
            counter = 0;
            state = HandDemoState::SlowOpen;
            SetupTimer(fastTimerPeriod);
        }
        
        for (int i = 0; i < FINGER_COUNT; i++)
        {
            if ((counter >> i) & 0x01)
                hand.OpenFinger(i);
            else
                hand.CloseFinger(i);
        }
        ++counter;
        break;
    case HandDemoState::SlowOpen:
        hand.OpenFingers(counter/static_cast<float>(slowMoveResolution));
        ++counter;
        if (counter >= slowMoveResolution) {
            counter = slowMoveResolution - 1;
            state = HandDemoState::SlowClose;
        }
        break;
    case HandDemoState::SlowClose:
        hand.OpenFingers(counter/static_cast<float>(slowMoveResolution));
        --counter;
        if (counter <= -1) {
            counter = 0;
            state = HandDemoState::Idle;
        }
        break;
    case HandDemoState::Idle:
        hand.CloseFingers();
        timer.Cancel();
        break;
    }
}
