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
private:
    void RunFSM();
    void TransitionToState(DemoState* newState);
    
private:
    IHand& hand;
    std::optional<infra::TimerRepeating> timer;
    DemoState* currentState;
};
