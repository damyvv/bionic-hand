// Servo Mock class

#pragma once

#include "drivers/Servo.hpp"
#include <gmock/gmock.h>

class ServoMock
    : public IServo
{
public:
    MOCK_METHOD(void, SetAngle, (float angle), (override));
};
