#include "HandFactory.hpp"
#include "drivers/Pca9685.hpp"
#include "Finger.hpp"

Hand HandFactory::CreateHand(Pca9685& pwmController)
{
    std::array<Finger, 5> fingers = {
        Finger(Servo(pwmController.GetChannel(0)), 35.0f, 140.0f),
        Finger(Servo(pwmController.GetChannel(1)), 90.0f, 15.0f),
        Finger(Servo(pwmController.GetChannel(2)), 95.0f, 15.0f),
        Finger(Servo(pwmController.GetChannel(3)), 95.0f, 15.0f),
        Finger(Servo(pwmController.GetChannel(4)), 95.0f, 15.0f)
    };

    return Hand(std::move(fingers));
}
