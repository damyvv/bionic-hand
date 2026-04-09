#include "Pca9685.hpp"

#include "infra/util/ReallyAssert.hpp"
#include <cmath>

static constexpr uint8_t MODE1_REGISTER = 0x00;
static constexpr uint8_t PRE_SCALE_REGISTER = 0xFE;
static constexpr uint8_t LED0_ON_L_REGISTER = 0x06;
static constexpr uint8_t LEDn_REGISTER_OFFSET = 4; // Each channel has 4 registers: ON_L, ON_H, OFF_L, OFF_H
static constexpr float INTERNAL_OSCILLATOR_FREQUENCY = 25000000.0; // 25 MHz
static constexpr uint16_t MAX_PWM_RESOLUTION = 4096; // 12-bit resolution

namespace
{
    template<std::size_t... I>
    std::array<Pca9685Channel, sizeof...(I)> MakeChannels(
        Pca9685& owner,
        std::index_sequence<I...>)
    {
        return { Pca9685Channel{ owner, static_cast<uint8_t>(I) }... };
    }
}

Pca9685::Pca9685(hal::I2cMaster& i2c, hal::I2cAddress address, uint16_t frequency)
    : i2c(i2c)
    , address(address)
    , channels(MakeChannels(*this, std::make_index_sequence<16>{}))
{
    PushInitializationSequence();
    SetFrequency(frequency);
    ProcessI2cMessageQueue();
}

Pca9685Channel& Pca9685::GetChannel(uint8_t channel)
{
    really_assert(channel < channels.size());
    return channels[channel];
}

void Pca9685::SetFrequency(uint16_t frequencyHz)
{
    this->frequency = frequencyHz;

    uint8_t prescale_value = static_cast<uint8_t>(std::round(INTERNAL_OSCILLATOR_FREQUENCY / (MAX_PWM_RESOLUTION * frequencyHz)) - 1);
    
    // PRE_SCALE = 0x79 to set the PWM frequency to 50 Hz
    i2cMessageQueue.push(Pca9685Message{ std::vector<uint8_t>{ PRE_SCALE_REGISTER, prescale_value }, nullptr });
}

void Pca9685::SetChannelPulseOn(uint8_t channel, uint16_t pulseOn)
{
    uint8_t onLow = pulseOn & 0xFF;
    uint8_t onHigh = (pulseOn >> 8) & 0xFF;
    
    uint8_t registerAddress = LED0_ON_L_REGISTER + (channel * LEDn_REGISTER_OFFSET);
    // The pulse is started at t=0, so the ON registers are set to 0 and the OFF registers are set to the pulse width (onLow/onHigh).
    i2cMessageQueue.push(Pca9685Message{ std::vector<uint8_t>{ registerAddress, 0, 0, onLow, onHigh }, nullptr });
    ProcessI2cMessageQueue();
}

void Pca9685::PushInitializationSequence()
{
    // MODE1 = 0x20 to set auto-increment and enable the oscillator (normal mode)
    i2cMessageQueue.push(Pca9685Message{ std::vector<uint8_t>{ MODE1_REGISTER, 0x20 }, nullptr });
}

void Pca9685::ProcessI2cMessageQueue()
{
    if (processingI2cQueue || i2cMessageQueue.empty()) {
        return;
    }
    processingI2cQueue = true;

    const Pca9685Message& message = i2cMessageQueue.front();
    i2c.SendData(address, message.data, hal::Action::stop, [this](hal::Result result, uint32_t numberOfBytesSent)
        {
            auto onSent = i2cMessageQueue.front().onSent;
            i2cMessageQueue.pop();
            processingI2cQueue = false;
            if (onSent) {
                onSent(result);
            }
            ProcessI2cMessageQueue();
        });
}
