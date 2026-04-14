#include "Pca9685.hpp"
#include "Pca9685Defines.hpp"

#include "infra/util/ReallyAssert.hpp"
#include <cmath>
#include "DriverUtils.hpp"

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

Pca9685::Pca9685(hal::I2cMaster& i2c, hal::I2cAddress address, hal::I2cErrorPolicy* errorPolicy, uint16_t frequency)
    : i2c(i2c)
    , address(address)
    , channels(MakeChannels(*this, std::make_index_sequence<PCA9685_CHANNELS>{}))
{
    if (errorPolicy != nullptr) {
        SetErrorPolicy(*errorPolicy);
    }
    PushInitializationSequence();
    SetFrequency(frequency);
    ProcessI2cMessageQueue();
}

Pca9685::Pca9685(hal::I2cMaster& i2c, hal::I2cAddress address, uint16_t frequency)
    : Pca9685(i2c, address, nullptr, frequency)
{
}

Pca9685::Pca9685(hal::I2cMaster& i2c, hal::I2cAddress address, hal::I2cErrorPolicy& errorPolicy, uint16_t frequency)
    : Pca9685(i2c, address, &errorPolicy, frequency)
{
}

Pca9685Channel& Pca9685::GetChannel(uint8_t channel)
{
    really_assert(channel < channels.size());
    return channels[channel];
}

void Pca9685::SetFrequency(uint16_t frequencyHz)
{
    if (i2cMessageQueue.full()) {
        // If the queue is full, we can't send the command to update the prescale value, so we shouldn't update the cached frequency and period until we can ensure the command is sent.
        assert(false);
        return;
    }

    really_assert(frequencyHz > 0);
    this->frequency = frequencyHz;
    periodInUs = DriverUtils::HertzToMicroseconds(frequencyHz);

    // Formula to calculate prescale value from PCA9685 datasheet
    const uint8_t prescale_value = static_cast<uint8_t>(std::round(INTERNAL_OSCILLATOR_FREQUENCY / (MAX_PWM_RESOLUTION * frequencyHz)) - 1);
    
    i2cMessageQueue.emplace_back(std::initializer_list<uint8_t>{PRE_SCALE_REGISTER, prescale_value});
    ProcessI2cMessageQueue();
}

void Pca9685::SetChannelPulseOn(uint8_t channel, uint16_t pulseOn, infra::Function<void(hal::Result)> onSent)
{
    if (i2cMessageQueue.full()) {
        // If the queue is full, we can't send the command to update the channel, so we shouldn't update the channel until we can ensure the command is sent.
        assert(false);
        return;
    }

    const uint8_t onLow = pulseOn & 0xFF;
    const uint8_t onHigh = (pulseOn >> 8) & 0xFF;
    
    const uint8_t registerAddress = LED0_ON_L_REGISTER + (channel * LEDn_REGISTER_OFFSET);
    // The pulse is started at t=0, so the ON registers are set to 0 and the OFF registers are set to the pulse width (onLow/onHigh).
    i2cMessageQueue.emplace_back(std::initializer_list<uint8_t>{registerAddress, 0, 0, onLow, onHigh}, onSent);
    ProcessI2cMessageQueue();
}

void Pca9685::SetErrorPolicy(hal::I2cErrorPolicy& policy)
{
    i2c.SetErrorPolicy(policy);
}

void Pca9685::ResetErrorPolicy()
{
    i2c.ResetErrorPolicy();
}

void Pca9685::PushInitializationSequence()
{
    really_assert(!i2cMessageQueue.full());

    // MODE1 = 0x20 to set auto-increment and enable the oscillator (normal mode)
    i2cMessageQueue.emplace_back(std::initializer_list<uint8_t>{MODE1_REGISTER, MODE1_AUTO_INCREMENT_BIT});
}

void Pca9685::ProcessI2cMessageQueue()
{
    if (processingI2cQueue || i2cMessageQueue.empty()) {
        return;
    }
    processingI2cQueue = true;

    const Pca9685Message& message = i2cMessageQueue.front();
    i2c.SendData(address, message.data.range(), hal::Action::stop, [this](hal::Result result, uint32_t)
        {
            auto onSent = i2cMessageQueue.front().onSent;
            i2cMessageQueue.pop_front();
            processingI2cQueue = false;
            ProcessI2cMessageQueue();

            // Process the callback after starting the next I2C transaction.
            if (onSent) {
                onSent(result);
            }
        });
}
