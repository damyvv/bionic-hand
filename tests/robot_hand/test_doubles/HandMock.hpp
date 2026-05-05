#pragma once

#include "Hand.hpp"
#include <gmock/gmock.h>

class HandMock
    : public IHand
{
public:
    MOCK_METHOD(bool, OpenFinger, (uint8_t fingerId, float percentage), (override));
    MOCK_METHOD(bool, OpenFinger, (uint8_t fingerId), (override));
    MOCK_METHOD(bool, CloseFinger, (uint8_t fingerId), (override));
    MOCK_METHOD(void, OpenFingers, (float percentage), (override));
    MOCK_METHOD(void, OpenFingers, (), (override));
    MOCK_METHOD(void, CloseFingers, (), (override));
    MOCK_METHOD(std::size_t, GetFingerCount, (), (const, override));
    MOCK_METHOD(std::optional<std::reference_wrapper<Finger>>, GetFinger, (uint8_t fingerId), (override));
};
