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
    return nextState;
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
    return nextState;
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
        demo.GetHand().CloseFingers();
    }
    else if (counter > demo.GetHand().GetFingerCount()) {
        return nextState;
    } else {
        demo.GetHand().OpenFinger((counter + 1) % demo.GetHand().GetFingerCount());
    }
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
    demo.GetHand().CloseFingers();
}

DemoState* WaveState::Update(HandDemo& demo)
{
    if (counter >= demo.GetHand().GetFingerCount() * waves * 2) {
        return nextState;
    }
    bool openFinger = (counter / demo.GetHand().GetFingerCount()) % 2 == 0;
    uint8_t fingerId = counter % demo.GetHand().GetFingerCount();
    demo.GetHand().OpenFinger(fingerId, openFinger ? 1.0f : 0.0f);
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
        return nextState;
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
        return nextState;
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

GameState::GameState()
    : counter(0)
{
    SetNextState(&IdleState::GetInstance());
}

GameState& GameState::GetInstance()
{
    static GameState instance;
    return instance;
}

void GameState::OnEntry(HandDemo& demo)
{
    counter = 0;
    demo.SetupTimer(slowTimerPeriod);
    demo.GetHand().CloseFingers();
}

DemoState* GameState::Update(HandDemo& demo)
{
    // Count down fingers
    if (counter < 3) {
        demo.GetHand().OpenFinger(counter+1);
        demo.GetSerialOutput().Write('1' + counter);
        demo.GetSerialOutput().Write("...\n");
    } else if (counter == 3) {
        auto time = demo.GetTimerService().Now();
        auto timeInMillis = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
        auto pick = timeInMillis % 3;

        demo.GetSerialOutput().Write("I choose ");
        switch (pick)
        {
        case 0:
            // Rock!
            demo.GetHand().CloseFingers();
            demo.GetSerialOutput().Write("Rock!\n");
            break;
        case 1:
            // Paper!
            demo.GetHand().OpenFingers();
            demo.GetSerialOutput().Write("Paper!\n");
            break;
        case 2:
            // Scissors!
            demo.GetHand().CloseFingers();
            demo.GetHand().OpenFinger(1);
            demo.GetHand().OpenFinger(2);
            demo.GetSerialOutput().Write("Scissors!\n");
            break;
        default:
            really_assert(false);
            break;
        }
        demo.GetSerialOutput().Write("I hope I won!\n");
    } else if (counter > 10) {
        // After some time, go back to idle state.
        return nextState;
    }

    ++counter;
    return nullptr;
}
