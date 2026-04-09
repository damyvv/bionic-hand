#pragma once

#include "hal/interfaces/PulseWidthModulation.hpp"

class Pca9685;

class Pca9685Channel : public hal::PulseWidthModulation
{
public:
    Pca9685Channel(Pca9685& pca9685, uint8_t channel);
    ~Pca9685Channel() = default;
    
    void SetDuty(uint8_t dutyPercent) override;
    void SetPulse(uint32_t pulseOn, uint32_t period) override;
    void Start() override;
    void Stop() override;
private:
    Pca9685& pca9685;
    uint8_t channel;
    uint32_t pulseOn;
};
