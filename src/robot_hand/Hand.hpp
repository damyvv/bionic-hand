#pragma once

#include "Finger.hpp"
#include <array>
#include <infra/util/ReallyAssert.hpp>

template<uint8_t NFingers = 5>
class Hand
{
public:
    explicit Hand(std::array<Finger, NFingers>&& fingers)
        : fingers(std::move(fingers)) {}
    ~Hand() = default;

    void OpenFinger(uint8_t fingerId, float percentage)
    {
        GetFinger(fingerId).Open(percentage);
    }
    void OpenFinger(uint8_t fingerId)
    {
        GetFinger(fingerId).Open();
    }
    void CloseFinger(uint8_t fingerId)
    {
        GetFinger(fingerId).Close();
    }

    void OpenFingers(float percentage)
    {
        for (auto& finger : fingers)
        {
            finger.Open(percentage);
        }
    }
    void OpenFingers()
    {
        for (auto& finger : fingers)
        {
            finger.Open();
        }
    }
    void CloseFingers()
    {
        for (auto& finger : fingers)
        {
            finger.Close();
        }
    }

    Finger& GetFinger(uint8_t fingerId)
    {
        really_assert(fingerId < NFingers);
        return fingers[fingerId];
    }
private:
    std::array<Finger, NFingers> fingers;
};
