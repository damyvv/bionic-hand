#include "Pca9685Channel.hpp"
#include "Pca9685.hpp"
#include "Pca9685Defines.hpp"

#include "infra/util/ReallyAssert.hpp"

Pca9685Channel::Pca9685Channel(Pca9685& pca9685, uint8_t channel)
    : pca9685(pca9685), channel(channel), pulseOn(0)
{
}

void Pca9685Channel::SetDuty(uint8_t dutyPercent)
{
    constexpr uint8_t MAX_DUTY_CYCLE = 100;

    if (dutyPercent > MAX_DUTY_CYCLE) {
        dutyPercent = MAX_DUTY_CYCLE;
    }

    const uint16_t pulseOn = (dutyPercent * MAX_PWM_RESOLUTION) / MAX_DUTY_CYCLE;
    
    this->pulseOn = pulseOn;
    pca9685.SetChannelPulseOn(channel, pulseOn);
}

void Pca9685Channel::SetPulse(uint32_t pulseOn, uint32_t period)
{
    const uint32_t periodInMicroseconds = pca9685.GetPeriodInMicroseconds();
    really_assert(period == periodInMicroseconds);

    this->pulseOn = pulseOn;
    pca9685.SetChannelPulseOn(channel, pulseOn);
}

void Pca9685Channel::Start()
{
    pca9685.SetChannelPulseOn(channel, pulseOn);
}

void Pca9685Channel::Stop()
{
    // There is no individual channel start/stop functionality in the PCA9685, so just mimic a stop by setting the pulse on time to 0
    pca9685.SetChannelPulseOn(channel, 0);
}
