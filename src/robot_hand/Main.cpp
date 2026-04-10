#include "hal_st/instantiations/NucleoUi.hpp"
#include "hal_st/instantiations/StmEventInfrastructure.hpp"
#include "hal_st/stm32fxxx/DefaultClockNucleoF429ZI.hpp"
#include "hal_st/stm32fxxx/I2cStm.hpp"
#include "HandFactory.hpp"
#include "HandDemo.hpp"

unsigned int hse_value = 8'000'000;

static main_::Nucleo144Ui ui;

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
    Hand hand = HandFactory::CreateHand(pwmController);

    // hand.OpenFingers(0);

    HandDemo demo(hand);
    demo.StartDemo();

    eventInfrastructure.Run();
    __builtin_unreachable();
}
