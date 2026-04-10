#pragma once

#include "IServo.hpp"
#include "hal/interfaces/PulseWidthModulation.hpp"

class Servo : public IServo
{
public:
    Servo() = default;
    explicit Servo(hal::PulseWidthModulation& pwm, uint16_t frequency = 50);
    Servo(const Servo&) = default;
    ~Servo() override = default;

    Servo& operator=(const Servo&) = default;

    void SetAngle(float angle) override;
private:
    hal::PulseWidthModulation* pwm = nullptr;
    uint16_t frequency = 50;
};
