#include "Finger.hpp"

#include <algorithm>

Finger::Finger(Servo servo, float openAngle, float closedAngle)
    : servo(servo)
    , openAngle(openAngle)
    , closedAngle(closedAngle)
{
}

void Finger::Open(float percentage)
{
    percentage = std::clamp(percentage, 0.0f, 1.0f);
    float angle = closedAngle + (openAngle - closedAngle) * percentage;
    servo.SetAngle(angle);
}

void Finger::Open()
{
    Open(1.0f);
}

void Finger::Close()
{
    Open(0.0f);
}
