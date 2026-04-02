#include "hal_st/instantiations/NucleoUi.hpp"
#include "hal_st/instantiations/StmEventInfrastructure.hpp"
#include "hal_st/stm32fxxx/DefaultClockNucleoF429ZI.hpp"
#include "services/util/DebugLed.hpp"
#include "hal_st/stm32fxxx/I2cStm.hpp"

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

    std::array<uint8_t, 2> data{ 0x00, 0x20 }; // MODE1 = 0x20 to set auto-increment and enable the oscillator (normal mode)
    i2c.SendData(pwmControllerAddress, data, hal::Action::stop, [&i2c, &pwmControllerAddress](hal::Result result, uint32_t numberOfBytesSent)
        {
            setDebugLedsError(result != hal::Result::complete);
            std::array<uint8_t, 2>data = { 0xFE, 0x79 }; // PRE_SCALE = 0x79 to set the PWM frequency to 50 Hz
            i2c.SendData(pwmControllerAddress, data, hal::Action::stop, [&i2c, &pwmControllerAddress](hal::Result result, uint32_t numberOfBytesSent)
                {
                    setDebugLedsError(result != hal::Result::complete);
                    
                    std::array<uint8_t, 1+2*2> data{ 0xFA, // Starting register address: ALL_LED_ON_L
                        0x00, 0x00, // On time
                        0xC0, 0x02, // Off time
                    };
                    i2c.SendData(pwmControllerAddress, data, hal::Action::stop, [&i2c, &pwmControllerAddress](hal::Result result, uint32_t numberOfBytesSent)
                        {
                            setDebugLedsError(result != hal::Result::complete);
                        });
                });
        });

    eventInfrastructure.Run();

    while(1) {}
}
