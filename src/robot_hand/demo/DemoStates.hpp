#pragma once

#include <cstdint>

class HandDemo;

class DemoState
{
protected:
    DemoState() = default;
public:
    virtual ~DemoState() = default;
    virtual void OnEntry(HandDemo& demo) {}
    virtual void OnExit(HandDemo& demo) {}
    virtual DemoState* Update(HandDemo& demo) = 0;
};

class OpeningFingersState : public DemoState
{
public:
    static OpeningFingersState& GetInstance();

    void OnEntry(HandDemo& demo) override;
    DemoState* Update(HandDemo& demo) override;

private:
    OpeningFingersState() = default;
};

class ClosingFingersState : public DemoState
{
public:
    static ClosingFingersState& GetInstance();

    void OnEntry(HandDemo& demo) override;
    DemoState* Update(HandDemo& demo) override;

private:
    ClosingFingersState() = default;
};

class CountFingersState : public DemoState
{
public:
    static CountFingersState& GetInstance();

    void OnEntry(HandDemo& demo) override;
    DemoState* Update(HandDemo& demo) override;

private:
    CountFingersState() = default;

    int counter = 0;
};

class WaveState : public DemoState
{
public:
    static WaveState& GetInstance();

    void OnEntry(HandDemo& demo) override;
    DemoState* Update(HandDemo& demo) override;

private:
    WaveState() = default;

    int counter = 0;
};

class SlowOpenState : public DemoState
{
public:
    static SlowOpenState& GetInstance();

    void OnEntry(HandDemo& demo) override;
    DemoState* Update(HandDemo& demo) override;

private:
    SlowOpenState() = default;

    int counter = 0;
};

class SlowCloseState : public DemoState
{
public:
    static SlowCloseState& GetInstance();

    void OnEntry(HandDemo& demo) override;
    DemoState* Update(HandDemo& demo) override;

private:
    SlowCloseState() = default;

    int counter = 0;
};

class IdleState : public DemoState
{
public:
    static IdleState& GetInstance();

    void OnEntry(HandDemo& demo) override;
    DemoState* Update(HandDemo& demo) override;

private:
    IdleState() = default;
};