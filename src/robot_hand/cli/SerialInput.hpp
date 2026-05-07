#pragma once

#include <infra/util/Function.hpp>
#include <infra/util/BoundedDeque.hpp>
#include <hal/interfaces/SerialCommunication.hpp>
#include "../utils/CriticalSectionGuard.hpp"

class ISerialInput
{
public:
    virtual ~ISerialInput() = default;
    virtual void ReceiveByte(infra::Function<void(uint8_t)> actionOnByteReceived) = 0;
};

template<std::size_t BufferSize>
class SerialInput : public ISerialInput
{
public:
    explicit SerialInput(hal::SerialCommunication& serial)
    {
        serial.ReceiveData([this](infra::ConstByteRange data) {
            DataReceived(data);
        });
    }

    void ReceiveByte(infra::Function<void(uint8_t)> actionOnByteReceived) override
    {
        this->actionOnByteReceived = actionOnByteReceived;
    }

private:
    void DataReceived(infra::ConstByteRange data) {
        CriticalSectionGuard guard;
        inputBuffer.insert(inputBuffer.end(), data.begin(), data.end());
        if (!processingData) {
            processingData = true;
            infra::EventDispatcher::Instance().Schedule([this]() {
                ProcessInput();
            });
        }
    }

    void ProcessInput() {
        while (true) {
            uint8_t byte;
            {
                CriticalSectionGuard guard;
                if (inputBuffer.empty()) {
                    processingData = false;
                    return;
                }
                byte = inputBuffer.front();
                inputBuffer.pop_front();
            }
            if (actionOnByteReceived) {
                actionOnByteReceived(byte);
            }
        }
    }

private:
    infra::Function<void(uint8_t)> actionOnByteReceived;
    infra::BoundedDeque<char>::WithMaxSize<BufferSize> inputBuffer;
    bool processingData = false;
};
