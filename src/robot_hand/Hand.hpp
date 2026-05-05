#pragma once

#include "Finger.hpp"
#include <array>
#include <infra/util/ReallyAssert.hpp>
#include <optional>
#include <functional>
#include <utility>

class IHand
{
public:
    virtual ~IHand() = default;
    virtual bool OpenFinger(uint8_t fingerId, float percentage) = 0;
    virtual bool OpenFinger(uint8_t fingerId) = 0;
    virtual bool CloseFinger(uint8_t fingerId) = 0;
    virtual void OpenFingers(float percentage) = 0;
    virtual void OpenFingers() = 0;
    virtual void CloseFingers() = 0;
    virtual std::size_t GetFingerCount() const = 0;
    virtual std::optional<std::reference_wrapper<Finger>> GetFinger(uint8_t fingerId) = 0;
};

template<uint8_t NFingers = 5>
class Hand : public IHand
{
public:
    explicit Hand(const std::array<std::reference_wrapper<Finger>, NFingers>& fingers)
        : fingers(fingers) {}
    ~Hand() = default;

    bool OpenFinger(uint8_t fingerId, float percentage) override
    {
        if (auto finger = GetFinger(fingerId))
        {
            finger->get().Open(percentage);
            return true;
        }
        return false;
    }
    bool OpenFinger(uint8_t fingerId) override
    {
        if (auto finger = GetFinger(fingerId))
        {
            finger->get().Open();
            return true;
        }
        return false;
    }
    bool CloseFinger(uint8_t fingerId) override
    {
        if (auto finger = GetFinger(fingerId))
        {
            finger->get().Close();
            return true;
        }
        return false;
    }

    void OpenFingers(float percentage) override
    {
        for (auto& finger : fingers)
        {
            finger.get().Open(percentage);
        }
    }
    void OpenFingers() override
    {
        for (auto& finger : fingers)
        {
            finger.get().Open();
        }
    }
    void CloseFingers() override
    {
        for (auto& finger : fingers)
        {
            finger.get().Close();
        }
    }

    std::size_t GetFingerCount() const override
    {
        return NFingers;
    }

    std::optional<std::reference_wrapper<Finger>> GetFinger(uint8_t fingerId) override
    {
        if (fingerId < NFingers)
        {
            return fingers[fingerId];
        }
        return std::nullopt;
    }
private:
    std::array<std::reference_wrapper<Finger>, NFingers> fingers;
};
