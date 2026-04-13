#include "Servo.hpp"
#include <algorithm>

#include "DriverUtils.hpp"

Servo::Servo(hal::PulseWidthModulation& pwm, uint16_t frequency)
    : pwm(&pwm)
{
    SetFrequency(frequency);
}

void Servo::SetFrequency(uint16_t frequency)
{
    really_assert(frequency > 0);
    this->frequency = frequency;
    periodInUs = DriverUtils::HertzToMicroseconds(frequency);
}

void Servo::SetAngle(float angle)
{
    constexpr uint16_t MIN_PULSE_WIDTH = 544; // the shortest pulse sent to a servo in microseconds
    constexpr uint16_t MAX_PULSE_WIDTH = 2400; // the longest pulse sent to a servo in microseconds
    constexpr float MIN_ANGLE = 0.0f;
    constexpr float MAX_ANGLE = 180.0f;

    angle = std::clamp(angle, MIN_ANGLE, MAX_ANGLE);

    const uint16_t pulseWidth = DriverUtils::lerp(MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, (angle - MIN_ANGLE) / (MAX_ANGLE - MIN_ANGLE));
    if (pwm != nullptr) {
        pwm->SetPulse(pulseWidth, periodInUs); // period based on frequency
    }
}
