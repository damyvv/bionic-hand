#pragma once

#include "Hand.hpp"
#include "drivers/IServo.hpp"
#include <utility>

template<std::size_t FingerCount>
class HandBuilder
{
public:
    HandBuilder() = default;
    ~HandBuilder() = default;

    void AttachServoToFinger(uint8_t fingerId, IServo& servo, float openAngle, float closedAngle)
    {
        really_assert(fingerId < FingerCount);
        really_assert(!fingers[fingerId].has_value());
        really_assert(!hand.has_value());
        fingers[fingerId].emplace(servo, openAngle, closedAngle);
    }

    Hand<FingerCount>& CreateHand() {
        if (!hand.has_value()) {
            for (int i = 0; i < FingerCount; ++i)
            {
                really_assert(fingers[i].has_value());
            }
            hand.emplace(MakeFingerReferences(std::make_index_sequence<FingerCount>{}));
        }
        return *hand;
    }

private:
    template<std::size_t... I>
    std::array<std::reference_wrapper<Finger>, FingerCount> MakeFingerReferences(std::index_sequence<I...>)
    {
        return { std::ref(fingers[I].value())... };
    }

    std::optional<Hand<FingerCount>> hand;
    std::array<std::optional<Finger>, FingerCount> fingers;
};
