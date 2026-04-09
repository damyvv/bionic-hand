#include "hal_st/instantiations/NucleoUi.hpp"
#include "hal_st/instantiations/StmEventInfrastructure.hpp"
#include "hal_st/stm32fxxx/DefaultClockNucleoF429ZI.hpp"
#include "services/util/DebugLed.hpp"
#include "hal_st/stm32fxxx/I2cStm.hpp"
#include "drivers/Pca9685.hpp"
#include "drivers/Pca9685Channel.hpp"
#include "drivers/Servo.hpp"

unsigned int hse_value = 8'000'000;

static main_::Nucleo144Ui ui;

void setDebugLedsError(bool error)
{
    ui.ledRed.Set(error);
    ui.ledGreen.Set(!error);
}

int main()
{
    HAL_Init();
    ConfigureDefaultClockNucleoF429ZI();

    static main_::StmEventInfrastructure eventInfrastructure;
    constexpr hal::I2cAddress pwmControllerAddress{ 0x40 };

    hal::GpioPinStm SDA{ hal::Port::B, 9 };
    hal::GpioPinStm SCL{ hal::Port::B, 8 };
    hal::I2cStm i2c{ 1, SCL, SDA };

    Pca9685 pwmController(i2c, pwmControllerAddress);
    Pca9685Channel channel0(pwmController, 1);
    Pca9685Channel channel1(pwmController, 2);
    Servo servo1(channel0);
    Servo servo2(channel1);

    servo1.SetAngle(45);
    servo2.SetAngle(90);

    eventInfrastructure.Run();
    __builtin_unreachable();
}
