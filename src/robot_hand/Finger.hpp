#pragma once

#include "drivers/Servo.hpp"

class Finger
{
public:
    Finger(Servo servo, float openAngle, float closedAngle);
    ~Finger() = default;

    void Open(float percentage);
    void Open();
    void Close();
private:
    Servo servo;
    float openAngle;
    float closedAngle;
};