#include "DemoStates.hpp"
#include "HandDemo.hpp"

constexpr uint32_t slowMoveResolution = 500;
constexpr infra::Duration slowTimerPeriod = std::chrono::milliseconds(350);
constexpr infra::Duration fastTimerPeriod = std::chrono::milliseconds(5);

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
    demo.GetHand().OpenFinger((counter + 1) % FINGER_COUNT);
    ++counter;
    if (counter == FINGER_COUNT) {
        return &CountBinaryState::GetInstance();
    }
    return nullptr;
}

CountBinaryState& CountBinaryState::GetInstance()
{
    static CountBinaryState instance;
    return instance;
}

void CountBinaryState::OnEntry(HandDemo& demo)
{
    counter = 0;
    demo.SetupTimer(slowTimerPeriod);
}

DemoState* CountBinaryState::Update(HandDemo& demo)
{
    if (counter == (1 << FINGER_COUNT)) {
        return &SlowOpenState::GetInstance();
    }
    
    for (int i = 0; i < FINGER_COUNT; i++)
    {
        if ((counter >> i) & 0x01)
            demo.GetHand().OpenFinger(i);
        else
            demo.GetHand().CloseFinger(i);
    }
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
        return &SlowCloseState::GetInstance();
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
        return &IdleState::GetInstance();
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