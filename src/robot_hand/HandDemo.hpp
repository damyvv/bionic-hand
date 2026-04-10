#pragma once

#include "Hand.hpp"
#include "infra/timer/Timer.hpp"

enum class HandDemoState;

class HandDemo
{
public:
    HandDemo(Hand& hand);
    ~HandDemo() = default;
    void StartDemo();
private:
    void RunFSM();
    void SetupTimer(std::chrono::milliseconds period);
private:
    Hand& hand;
    infra::TimerRepeating timer;
    HandDemoState state;
    int counter;
};
