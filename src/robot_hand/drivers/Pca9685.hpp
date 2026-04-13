#pragma once

#include "hal/interfaces/I2c.hpp"
#include "Pca9685Channel.hpp"
#include <cstdint>
#include <queue>
#include <array>

#ifndef PCA9685_CHANNELS
#define PCA9685_CHANNELS 16
#endif

struct Pca9685Message
{
    std::vector<uint8_t> data;
};

class Pca9685
{
public:
    Pca9685(hal::I2cMaster& i2c, hal::I2cAddress address, uint16_t frequency = 50);
    ~Pca9685() = default;

    Pca9685(const Pca9685& other) = delete;
    Pca9685& operator=(const Pca9685& other) = delete;
    
    Pca9685Channel& GetChannel(uint8_t channel);
    
    void SetFrequency(uint16_t frequencyHz);
    inline uint16_t GetFrequency() const { return frequency; }
    uint32_t GetPeriodInMicroseconds() const;
    
    void SetChannelPulseOn(uint8_t channel, uint16_t pulseOn);

    void SetErrorPolicy(hal::I2cErrorPolicy& policy);
    void ResetErrorPolicy();

private:
    void PushInitializationSequence();
    void ProcessI2cMessageQueue();
private:
    hal::I2cMaster& i2c;
    hal::I2cAddress address;

    uint16_t frequency;

    std::array<Pca9685Channel, PCA9685_CHANNELS> channels;
    std::queue<Pca9685Message> i2cMessageQueue;
    bool processingI2cQueue = false;
};
