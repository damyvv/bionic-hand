#pragma once

#include "infra/util/Function.hpp"
#include <string_view>
#include "SerialInput.hpp"

class ISerialLineSource
{
public:
    virtual ~ISerialLineSource() = default;
    virtual void ReceiveLine(infra::Function<void(std::string_view)> actionOnLineReceived) = 0;
};

template<std::size_t BufferSize>
class SerialLineSource : public ISerialLineSource
{
private:
    static constexpr char deleteCharacter = 0x7F;
public:
    SerialLineSource() = default;
    explicit SerialLineSource(ISerialInput& serial)
    {
        serial.ReceiveByte([this](uint8_t byte) {
            SendByte(byte);
        });
    }

    void ReceiveLine(infra::Function<void(std::string_view)> actionOnLineReceived) override
    {
        this->actionOnLineReceived = actionOnLineReceived;
    }

    void SendByte(uint8_t byte)
    {
        ProcessCharacter(static_cast<char>(byte));
    }

private:
    void ProcessCharacter(char character)
    {
        const bool isTerminator = (character == '\n' || character == '\r');
        const bool previousWasTerminator = (lastCharacter == '\n' || lastCharacter == '\r');
        const bool isMixedPair = previousWasTerminator && (character != lastCharacter);

        if (isTerminator)
        {
            if (isMixedPair) {
                lastCharacter = 0;
                return;
            } else {
                EmitLine();
            }
        }
        else if (character == deleteCharacter)
        {
            if (!inputBuffer.empty()) {
                inputBuffer.pop_back();
            }
        }
        else if (!inputBuffer.full())
        {
            inputBuffer.push_back(character);
        }

        lastCharacter = character;
    }

    void EmitLine()
    {
        if (actionOnLineReceived) {
            actionOnLineReceived(std::string_view(inputBuffer.begin(), inputBuffer.size()));
        }
        inputBuffer.clear();
    }

private:
    infra::BoundedString::WithStorage<BufferSize> inputBuffer;
    infra::Function<void(std::string_view)> actionOnLineReceived;
    char lastCharacter = 0;
};
