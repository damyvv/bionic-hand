#include "HandDemo.hpp"

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

HandDemo::HandDemo(Hand& hand)
    : hand(hand)
    , timer()
    , state(HandDemoState::Idle)
{
}

void HandDemo::StartDemo()
{
    counter = 0;
    state = HandDemoState::OpeningFingers;
    SetupTimer(std::chrono::milliseconds(350));
}

void HandDemo::SetupTimer(std::chrono::milliseconds period)
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
        hand.OpenFinger(static_cast<FingerId>((counter + 1) % 5));
        counter = counter + 1;
        if (counter == 5) {
            state = HandDemoState::CountBinary;
            counter = 0;
        }
        break;
    case HandDemoState::CountBinary:
        for (int i = 0; i < 5; i++)
        {
            if ((counter >> i) & 0x01)
                hand.OpenFinger(static_cast<FingerId>(i));
            else
                hand.CloseFinger(static_cast<FingerId>(i));
        }
        counter = counter + 1;
        if (counter == 32) {
            counter = 0;
            state = HandDemoState::SlowOpen;
            SetupTimer(std::chrono::milliseconds(5));
        }
        break;
    case HandDemoState::SlowOpen:
        hand.OpenFingers(counter/500.f);
        counter = counter + 1;
        if (counter >= 501) {
            counter = 500;
            state = HandDemoState::SlowClose;
        }
        break;
    case HandDemoState::SlowClose:
        hand.OpenFingers(counter/500.f);
        counter = counter - 1;
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
