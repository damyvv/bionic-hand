#include <hal_st/instantiations/NucleoUi.hpp>
#include <hal_st/instantiations/StmEventInfrastructure.hpp>
#include <hal_st/stm32fxxx/DefaultClockNucleoF429ZI.hpp>
#include <hal_st/stm32fxxx/I2cStm.hpp>
#include <hal_st/stm32fxxx/UartStm.hpp>
#include "HandFactory.hpp"
#include "demo/HandDemo.hpp"
#include "cli/SerialLineSource.hpp"
#include "cli/SerialOutput.hpp"
#include "cli/CommandLineApp.hpp"

unsigned int hse_value = 8'000'000;

static main_::Nucleo144Ui ui;

class I2CErrorHandler : public hal::I2cErrorPolicy
{
public:
    I2CErrorHandler() = default;
    ~I2CErrorHandler() = default;

    void HandleError() { ui.ledRed.Set(true); }
    virtual void DeviceNotFound() override { HandleError(); }
    virtual void BusError() override { HandleError(); }
    virtual void ArbitrationLost() override { HandleError(); }
};

int main()
{
    HAL_Init();
    ConfigureDefaultClockNucleoF429ZI();

    static main_::StmEventInfrastructure eventInfrastructure;
    constexpr hal::I2cAddress pwmControllerAddress{ 0x40 };

    static hal::GpioPinStm usart3tx{ hal::Port::D, 8 };
    static hal::GpioPinStm usart3rx{ hal::Port::D, 9 };
    static hal::UartStm uart{ 3, usart3tx, usart3rx };

    SerialOutput<1024> serialOutput(uart);
    SerialLineSource<128> serialLineSource(uart);
    CommandLineApp commandLineApp(serialLineSource, serialOutput);

    static hal::GpioPinStm SDA{ hal::Port::B, 9 };
    static hal::GpioPinStm SCL{ hal::Port::B, 8 };
    static hal::I2cStm i2c{ 1, SCL, SDA };
    static I2CErrorHandler errorHandler;
    static Pca9685 pwmController(i2c, pwmControllerAddress, errorHandler);
    static std::array<Servo, FINGER_COUNT> servos;
    Hand<FINGER_COUNT> hand = HandFactory::CreateHand(pwmController, servos);

    HandDemo demo(hand);
    demo.StartDemo();

    eventInfrastructure.Run();
    __builtin_unreachable();
}
