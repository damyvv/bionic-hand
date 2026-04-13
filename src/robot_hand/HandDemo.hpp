#pragma once

#include "Hand.hpp"
#include "infra/timer/Timer.hpp"

enum class HandDemoState;

class HandDemo
{
public:
    HandDemo(Hand<FINGER_COUNT>& hand);
    ~HandDemo() = default;
    void StartDemo();
private:
    void RunFSM();
    void SetupTimer(infra::Duration period);
private:
    Hand<FINGER_COUNT>& hand;
    infra::TimerRepeating timer;
    HandDemoState state;
    int counter;
};
