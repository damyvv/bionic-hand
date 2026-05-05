#include "HandDemo.hpp"
#include "DemoStates.hpp"
#include "infra/timer/Timer.hpp"

HandDemo::HandDemo(IHand& hand)
    : hand(hand)
    , currentState(&IdleState::GetInstance())
{
}

void HandDemo::StartDemo()
{
    TransitionToState(&OpeningFingersState::GetInstance());
}

void HandDemo::StopDemo()
{
    TransitionToState(&IdleState::GetInstance());
}

void HandDemo::SetupTimer(infra::Duration period)
{
    if (!timer.has_value()) {
        // Lazily construct the timer when needed.
        timer.emplace();
    }

    timer->Cancel();
    timer->Start(period, [this]() { RunFSM(); });
}

void HandDemo::CancelTimer()
{
    if (timer.has_value()) {
        timer->Cancel();
    }
}

void HandDemo::RunFSM()
{
    DemoState* nextState = currentState->Update(*this);
    if (nextState != nullptr) {
        TransitionToState(nextState);
    }
}

void HandDemo::TransitionToState(DemoState* newState)
{
    if (currentState)
        currentState->OnExit(*this);
    
    currentState = newState;
    currentState->OnEntry(*this);
}
