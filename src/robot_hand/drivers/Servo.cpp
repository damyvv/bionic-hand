#include "Servo.hpp"
#include <algorithm>

static constexpr uint32_t FrequencyToPeriod(uint16_t frequency)
{
    return 1'000'000 / frequency; // period in microseconds
}

Servo::Servo(hal::PulseWidthModulation& pwm, uint16_t frequency)
    : pwm(&pwm)
    , frequency(frequency)
{
}

void Servo::SetAngle(float angle)
{
    constexpr uint16_t MIN_PULSE_WIDTH = 544; // the shortest pulse sent to a servo in microseconds
    constexpr uint16_t MAX_PULSE_WIDTH = 2400; // the longest pulse sent to a servo in microseconds
    constexpr float MIN_ANGLE = 0.0f;
    constexpr float MAX_ANGLE = 180.0f;

    angle = std::clamp(angle, MIN_ANGLE, MAX_ANGLE);

    const uint16_t pulseWidth = MIN_PULSE_WIDTH + static_cast<uint16_t>(((MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) * angle) / (MAX_ANGLE - MIN_ANGLE));
    if (pwm != nullptr) {
        pwm->SetPulse(pulseWidth, FrequencyToPeriod(frequency)); // period based on frequency
    }
}
