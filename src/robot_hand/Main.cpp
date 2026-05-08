#include <hal_st/instantiations/NucleoUi.hpp>
#include <hal_st/instantiations/StmEventInfrastructure.hpp>
#include <hal_st/stm32fxxx/DefaultClockNucleoF429ZI.hpp>
#include <hal_st/stm32fxxx/I2cStm.hpp>
#include <hal_st/stm32fxxx/UartStm.hpp>
#include "HandBuilder.hpp"
#include "drivers/Pca9685.hpp"
#include "drivers/Servo.hpp"
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

    constexpr std::size_t fingerCount = 5;

    static main_::StmEventInfrastructure eventInfrastructure;
    constexpr hal::I2cAddress pwmControllerAddress{ 0x40 };

    static hal::GpioPinStm usart3tx{ hal::Port::D, 8 };
    static hal::GpioPinStm usart3rx{ hal::Port::D, 9 };
    static hal::UartStm uart{ 3, usart3tx, usart3rx };
    static SerialOutput<1024> serialOutput(uart);
    static SerialInput<32> serialInput(uart);
    static SerialLineSource<128> serialLineSource;
    serialInput.ReceiveByte([](uint8_t byte) {
        // Echo back the received byte.
        serialOutput.Write(byte);
        // Also send it to the SerialLineSource to be processed into lines.
        serialLineSource.SendByte(byte);
    });

    static hal::GpioPinStm SDA{ hal::Port::B, 9 };
    static hal::GpioPinStm SCL{ hal::Port::B, 8 };
    static hal::I2cStm i2c{ 1, SCL, SDA };
    static I2CErrorHandler errorHandler;
    static Pca9685 pwmController(i2c, pwmControllerAddress, errorHandler);
    static std::array<Servo, fingerCount> servos{
        Servo(pwmController.GetChannel(0)),
        Servo(pwmController.GetChannel(1)),
        Servo(pwmController.GetChannel(2)),
        Servo(pwmController.GetChannel(3)),
        Servo(pwmController.GetChannel(4))
    };

    static HandBuilder<fingerCount> handBuilder;
    handBuilder.AttachServoToFinger(0, servos[0], 35.0f, 120.0f);
    handBuilder.AttachServoToFinger(1, servos[1], 90.0f, 15.0f);
    handBuilder.AttachServoToFinger(2, servos[2], 95.0f, 15.0f);
    handBuilder.AttachServoToFinger(3, servos[3], 95.0f, 15.0f);
    handBuilder.AttachServoToFinger(4, servos[4], 95.0f, 15.0f);
    IHand& hand = handBuilder.CreateHand();

    HandDemo demo(hand, eventInfrastructure.systemTick, serialOutput);

    CommandLineApp commandLineApp(serialLineSource, serialOutput, hand, demo);
    
    demo.StartDemo();

    eventInfrastructure.Run();
    __builtin_unreachable();
}
