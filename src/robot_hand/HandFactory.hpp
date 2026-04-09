#pragma once

#include "Hand.hpp"
#include "drivers/Pca9685.hpp"

class HandFactory
{
public:
    static Hand CreateHand(Pca9685& pwmController);
};
