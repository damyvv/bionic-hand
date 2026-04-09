#include "Pca9685Channel.hpp"
#include "infra/util/ReallyAssert.hpp"

Pca9685Channel::Pca9685Channel(Pca9685& pca9685, uint8_t channel)
    : pca9685(pca9685), channel(channel)
{
}

void Pca9685Channel::SetDuty(uint8_t dutyPercent)
{
    if (dutyPercent > 100) {
        dutyPercent = 100;
    }

    uint32_t periodInMicroseconds = pca9685.GetPeriodInMicroseconds();
    pulseOn = (periodInMicroseconds * dutyPercent) / 100;

    pca9685.SetChannelPulseOn(channel, pulseOn);
}

void Pca9685Channel::SetPulse(uint32_t pulseOn, uint32_t period)
{
    uint32_t periodInMicroseconds = pca9685.GetPeriodInMicroseconds();
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
