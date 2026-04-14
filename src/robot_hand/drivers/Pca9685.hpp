#pragma once

#include "hal/interfaces/I2c.hpp"
#include "Pca9685Channel.hpp"
#include "infra/util/BoundedDeque.hpp"
#include "infra/util/BoundedVector.hpp"
#include <initializer_list>
#include <cstdint>
#include <array>

#ifndef PCA9685_CHANNELS
#define PCA9685_CHANNELS 16
#endif

constexpr size_t PCA9685_I2C_MESSAGE_MAX_SIZE = 5;
constexpr size_t PCA9685_MAX_MESSAGES_IN_QUEUE = 10;

struct Pca9685Message
{
    infra::BoundedVector<uint8_t>::WithMaxSize<PCA9685_I2C_MESSAGE_MAX_SIZE> data;
    infra::Function<void(hal::Result)> onSent;

    Pca9685Message(std::initializer_list<uint8_t> bytes, infra::Function<void(hal::Result)> onSent = nullptr)
        : data(bytes)
        , onSent(onSent)
    {
        really_assert(bytes.size() <= PCA9685_I2C_MESSAGE_MAX_SIZE);
    }
};

class Pca9685
{
private:
    Pca9685(hal::I2cMaster& i2c, hal::I2cAddress address, hal::I2cErrorPolicy* errorPolicy, uint16_t frequency);
public:
    Pca9685(hal::I2cMaster& i2c, hal::I2cAddress address, uint16_t frequency = 50);
    Pca9685(hal::I2cMaster& i2c, hal::I2cAddress address, hal::I2cErrorPolicy& errorPolicy, uint16_t frequency = 50);
    ~Pca9685() = default;

    Pca9685(const Pca9685& other) = delete;
    Pca9685& operator=(const Pca9685& other) = delete;
    
    Pca9685Channel& GetChannel(uint8_t channel);
    
    void SetFrequency(uint16_t frequencyHz);
    inline uint16_t GetFrequency() const { return frequency; }
    inline uint32_t GetPeriodInMicroseconds() const { return periodInUs; }
    
    void SetChannelPulseOn(uint8_t channel, uint16_t pulseOn, infra::Function<void(hal::Result)> onSent = nullptr);

    void SetErrorPolicy(hal::I2cErrorPolicy& policy);
    void ResetErrorPolicy();

private:
    void PushInitializationSequence();
    void ProcessI2cMessageQueue();
private:
    hal::I2cMaster& i2c;
    hal::I2cAddress address;

    uint16_t frequency;
    uint32_t periodInUs;

    std::array<Pca9685Channel, PCA9685_CHANNELS> channels;
    infra::BoundedDeque<Pca9685Message>::WithMaxSize<PCA9685_MAX_MESSAGES_IN_QUEUE> i2cMessageQueue;
    bool processingI2cQueue = false;
};
