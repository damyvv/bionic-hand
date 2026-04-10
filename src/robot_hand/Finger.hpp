#pragma once

#include "drivers/Servo.hpp"

class Finger
{
public:
    Finger(IServo& servo, float openAngle, float closedAngle);
    ~Finger() = default;

    void Open(float percentage);
    void Open();
    void Close();
private:
    IServo& servo;
    float openAngle;
    float closedAngle;
};