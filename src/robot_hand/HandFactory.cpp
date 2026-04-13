#include "HandFactory.hpp"

#include "drivers/Pca9685.hpp"
#include "drivers/Servo.hpp"
#include "Finger.hpp"

Hand<FINGER_COUNT> HandFactory::CreateHand(Pca9685& pwmController, std::array<Servo, FINGER_COUNT>& servos)
{
    for (int i = 0; i < FINGER_COUNT; ++i)
    {
        servos[i] = Servo(pwmController.GetChannel(i));
    }

    return Hand<FINGER_COUNT>({
        Finger(servos[0], 35.0f, 120.0f),
        Finger(servos[1], 90.0f, 15.0f),
        Finger(servos[2], 95.0f, 15.0f),
        Finger(servos[3], 95.0f, 15.0f),
        Finger(servos[4], 95.0f, 15.0f)
    });
}
