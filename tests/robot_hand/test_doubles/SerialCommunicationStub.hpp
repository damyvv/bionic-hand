#pragma once

#include "hal/interfaces/SerialCommunication.hpp"

#include <vector>

class SerialCommunicationStub
    : public hal::SerialCommunication
{
public:
    void SendData(infra::ConstByteRange data, infra::Function<void()> actionOnCompletion) override
    {
        lastSend.assign(data.begin(), data.end());
        if (actionOnCompletion)
            actionOnCompletion();
    }

    void ReceiveData(infra::Function<void(infra::ConstByteRange data)> dataReceived) override
    {
        this->dataReceived = dataReceived;
    }

    void EmitData(infra::ConstByteRange data)
    {
        if (dataReceived)
            dataReceived(data);
    }

    bool HasReceiveHandler() const
    {
        return static_cast<bool>(dataReceived);
    }

public:
    infra::Function<void(infra::ConstByteRange data)> dataReceived;
    std::vector<uint8_t> lastSend;
};
