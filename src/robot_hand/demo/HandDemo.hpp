#pragma once

#include "../Hand.hpp"
#include "infra/timer/Timer.hpp"
#include "infra/timer/TimerService.hpp"
#include <optional>
#include "../cli/SerialOutput.hpp"

class DemoState;

class HandDemo
{
public:
    HandDemo(IHand& hand, const infra::TimerService& timerService, ISerialOutput& serialOutput);
    ~HandDemo() = default;
    void StartDemo();
    void StopDemo();
    
    IHand& GetHand() { return hand; }
    void SetupTimer(infra::Duration period);
    void CancelTimer();
    void TransitionToState(DemoState* newState);
    void GetCurrentState(DemoState*& state) { state = currentState; }
    const infra::TimerService& GetTimerService() const { return timerService; }
    ISerialOutput& GetSerialOutput() { return serialOutput; }
private:
    void RunFSM();
    
private:
    IHand& hand;
    std::optional<infra::TimerRepeating> timer;
    DemoState* currentState;
    const infra::TimerService& timerService;
    ISerialOutput& serialOutput;
};
