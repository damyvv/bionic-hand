#pragma once

#include "hal/interfaces/PulseWidthModulation.hpp"

class Servo
{
public:
    Servo(hal::PulseWidthModulation& pwm, uint16_t frequency = 50);
    ~Servo() = default;

    void SetAngle(float angle);
private:
    hal::PulseWidthModulation& pwm;
    uint16_t frequency;
};
