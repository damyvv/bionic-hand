#pragma once

#include "../Hand.hpp"
#include "infra/timer/Timer.hpp"
#include <optional>

class DemoState;

class HandDemo
{
public:
    HandDemo(Hand<FINGER_COUNT>& hand);
    ~HandDemo() = default;
    void StartDemo();
    
    Hand<FINGER_COUNT>& GetHand() { return hand; }
    void SetupTimer(infra::Duration period);
    void CancelTimer();
private:
    void RunFSM();
    void TransitionToState(DemoState* newState);
    
private:
    Hand<FINGER_COUNT>& hand;
    std::optional<infra::TimerRepeating> timer;
    DemoState* currentState;
};
