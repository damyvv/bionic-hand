#pragma once

#include "Finger.hpp"
#include <array>
#include <infra/util/ReallyAssert.hpp>

enum class FingerId
{
    Thumb,
    Index,
    Middle,
    Ring,
    Little
};

class Hand
{
public:
    explicit Hand(std::array<Finger, 5>&& fingers);
    ~Hand() = default;

    void OpenFinger(FingerId fingerId, float percentage);
    void OpenFinger(FingerId fingerId);
    void CloseFinger(FingerId fingerId);

    void OpenFingers(float percentage);
    void OpenFingers();
    void CloseFingers();

    Finger& GetFinger(FingerId fingerId);
private:
    std::array<Finger, 5> fingers;
};
