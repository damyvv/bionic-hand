#pragma once

#include "Hand.hpp"
#include "drivers/Pca9685.hpp"
#include "drivers/Servo.hpp"

class HandFactory
{
public:
    static Hand CreateHand(Pca9685& pwmController, std::array<Servo, 5>& servos);
};
