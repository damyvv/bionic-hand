#include "DemoStates.hpp"
#include "HandDemo.hpp"

constexpr uint32_t slowMoveResolution = 500;
constexpr infra::Duration slowTimerPeriod = std::chrono::milliseconds(500);
constexpr infra::Duration fastTimerPeriod = std::chrono::milliseconds(5);
constexpr infra::Duration waveTimerPeriod = std::chrono::milliseconds(75);

OpeningFingersState& OpeningFingersState::GetInstance()
{
    static OpeningFingersState instance;
    return instance;
}

void OpeningFingersState::OnEntry(HandDemo& demo)
{
    demo.SetupTimer(slowTimerPeriod);
}

DemoState* OpeningFingersState::Update(HandDemo& demo)
{
    demo.GetHand().OpenFingers();
    return &ClosingFingersState::GetInstance();
}

ClosingFingersState& ClosingFingersState::GetInstance()
{
    static ClosingFingersState instance;
    return instance;
}

void ClosingFingersState::OnEntry(HandDemo& demo)
{
    demo.SetupTimer(slowTimerPeriod);
}

DemoState* ClosingFingersState::Update(HandDemo& demo)
{
    demo.GetHand().CloseFingers();
    return &CountFingersState::GetInstance();
}

CountFingersState& CountFingersState::GetInstance()
{
    static CountFingersState instance;
    return instance;
}

void CountFingersState::OnEntry(HandDemo& demo)
{
    counter = 0;
    demo.SetupTimer(slowTimerPeriod);
}

DemoState* CountFingersState::Update(HandDemo& demo)
{
    if (counter == demo.GetHand().GetFingerCount()) {
        return &WaveState::GetInstance();
    }
    demo.GetHand().OpenFinger((counter + 1) % demo.GetHand().GetFingerCount());
    ++counter;
    return nullptr;
}

WaveState& WaveState::GetInstance()
{
    static WaveState instance;
    return instance;
}

void WaveState::OnEntry(HandDemo& demo)
{
    counter = 0;
    demo.SetupTimer(waveTimerPeriod);
}

DemoState* WaveState::Update(HandDemo& demo)
{
    constexpr int totalWaves = 2;

    if (counter >= demo.GetHand().GetFingerCount() * totalWaves * 2) {
        return &SlowCloseState::GetInstance();
    }
    bool closeFinger = (counter / demo.GetHand().GetFingerCount()) % 2 == 0;
    uint8_t fingerId = counter % demo.GetHand().GetFingerCount();
    demo.GetHand().OpenFinger(fingerId, closeFinger ? 0.0f : 1.0f);
    ++counter;
    return nullptr;
}

SlowOpenState& SlowOpenState::GetInstance()
{
    static SlowOpenState instance;
    return instance;
}

void SlowOpenState::OnEntry(HandDemo& demo)
{
    counter = 0;
    demo.SetupTimer(fastTimerPeriod);
}

DemoState* SlowOpenState::Update(HandDemo& demo)
{
    demo.GetHand().OpenFingers(counter / static_cast<float>(slowMoveResolution));
    ++counter;
    if (counter >= slowMoveResolution) {
        return &IdleState::GetInstance();
    }
    return nullptr;
}

SlowCloseState& SlowCloseState::GetInstance()
{
    static SlowCloseState instance;
    return instance;
}

void SlowCloseState::OnEntry(HandDemo& demo)
{
    counter = slowMoveResolution - 1;
    demo.SetupTimer(fastTimerPeriod);
}

DemoState* SlowCloseState::Update(HandDemo& demo)
{
    demo.GetHand().OpenFingers(counter / static_cast<float>(slowMoveResolution));
    --counter;
    if (counter <= -1) {
        return &SlowOpenState::GetInstance();
    }
    return nullptr;
}

IdleState& IdleState::GetInstance()
{
    static IdleState instance;
    return instance;
}

void IdleState::OnEntry(HandDemo& demo)
{
    demo.GetHand().CloseFingers();
    demo.CancelTimer();
}

DemoState* IdleState::Update(HandDemo& demo)
{
    // Unused parameter
    static_cast<void>(demo);
    
    return nullptr;
}