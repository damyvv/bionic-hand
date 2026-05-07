#pragma once

#include "cli/SerialInput.hpp"

#include <array>

class SerialInputStub
    : public ISerialInput
{
public:
    void ReceiveByte(infra::Function<void(uint8_t)> actionOnByteReceived) override
    {
        firstHandler = actionOnByteReceived;
    }

    void EmitByte(uint8_t byte)
    {
        if (firstHandler)
            firstHandler(byte);
    }

    template<std::size_t Size>
    void EmitBytes(const std::array<uint8_t, Size>& data)
    {
        for (auto byte : data)
            EmitByte(byte);
    }

    bool HasReceiveHandler() const
    {
        return static_cast<bool>(firstHandler);
    }

private:
    infra::Function<void(uint8_t)> firstHandler;
};
