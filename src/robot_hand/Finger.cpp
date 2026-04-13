#include "Finger.hpp"

#include <algorithm>
#include "drivers/DriverUtils.hpp"

Finger::Finger(IServo& servo, float openAngle, float closedAngle)
    : servo(servo)
    , openAngle(openAngle)
    , closedAngle(closedAngle)
{
}

void Finger::Open(float percentage)
{
    percentage = std::clamp(percentage, 0.0f, 1.0f);
    const float angle = DriverUtils::lerp(closedAngle, openAngle, percentage);
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
