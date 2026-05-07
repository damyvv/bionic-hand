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
public:
    explicit SerialLineSource(ISerialInput& serial)
    {
        serial.ReceiveByte([this](uint8_t byte) {
            ProcessCharacter(static_cast<char>(byte));
        });
    }

    void ReceiveLine(infra::Function<void(std::string_view)> actionOnLineReceived) override
    {
        this->actionOnLineReceived = actionOnLineReceived;
    }
private:
    void ProcessCharacter(char character)
    {
        if (character == '\n') {
            if (lastCharacter == '\r') {
                // We already processed the line when we received the '\r', so just ignore the '\n'.
                lastCharacter = 0;
            } else {
                EmitLine();
            }
        }
        else if (character == '\r') {
            if (lastCharacter == '\n') {
                // We already processed the line when we received the '\n', so just ignore the '\r'.
                lastCharacter = 0;
            } else {
                EmitLine();
            }
        }
        else {
            if (inputBuffer.full()) {
                // The line is too long to fit in the buffer. We will truncate it, but this should not happen in normal operation.
                assert(false);
            }
            else {
                inputBuffer.push_back(character);
                lastCharacter = character;
            }
        }
    }

    void EmitLine()
    {
        if (actionOnLineReceived) {
            actionOnLineReceived(std::string_view(inputBuffer.begin(), inputBuffer.size()));
        }
        inputBuffer.clear();
        lastCharacter = 0;
    }

private:
    infra::BoundedString::WithStorage<BufferSize> inputBuffer;
    infra::Function<void(std::string_view)> actionOnLineReceived;
    char lastCharacter = 0;
};
