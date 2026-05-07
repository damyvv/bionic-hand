#pragma once

#include "cli/SerialLineSource.hpp"

class SerialLineSourceStub
    : public ISerialLineSource
{
public:
    void ReceiveLine(infra::Function<void(std::string_view)> actionOnLineReceived) override
    {
        this->actionOnLineReceived = actionOnLineReceived;
    }

    void ReceiveByte(infra::Function<void(uint8_t)> actionOnByteReceived) override
    {
        this->actionOnByteReceived = actionOnByteReceived;
    }

    void EmitLine(std::string_view line)
    {
        if (actionOnLineReceived)
            actionOnLineReceived(line);
    }

    void EmitByte(uint8_t byte)
    {
        if (actionOnByteReceived)
            actionOnByteReceived(byte);
    }

    bool HasLineHandler() const
    {
        return static_cast<bool>(actionOnLineReceived);
    }

    bool HasByteHandler() const
    {
        return static_cast<bool>(actionOnByteReceived);
    }

private:
    infra::Function<void(std::string_view)> actionOnLineReceived;
    infra::Function<void(uint8_t)> actionOnByteReceived;
};
