#include "Servo.hpp"

static constexpr uint32_t FrequencyToPeriod(uint16_t frequency)
{
    return 1000000 / frequency; // period in microseconds
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

    if (angle > 180.0f) {
        angle = 180.0f;
    }
    else if (angle < 0.0f) {
        angle = 0.0f;
    }

    const uint16_t pulseWidth = MIN_PULSE_WIDTH + static_cast<uint16_t>(((MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) * angle) / 180.0f);
    if (pwm != nullptr) {
        pwm->SetPulse(pulseWidth, FrequencyToPeriod(frequency)); // period based on frequency
    }
}
