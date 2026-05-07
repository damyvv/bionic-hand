#pragma once

#include "../Hand.hpp"
#include "infra/timer/Timer.hpp"
#include <optional>

class DemoState;

class HandDemo
{
public:
    HandDemo(IHand& hand);
    ~HandDemo() = default;
    void StartDemo();
    void StopDemo();
    
    IHand& GetHand() { return hand; }
    void SetupTimer(infra::Duration period);
    void CancelTimer();
    void TransitionToState(DemoState* newState);
    void GetCurrentState(DemoState*& state) { state = currentState; }
private:
    void RunFSM();
    
private:
    IHand& hand;
    std::optional<infra::TimerRepeating> timer;
    DemoState* currentState;
};
