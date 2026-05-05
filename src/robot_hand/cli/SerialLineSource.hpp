#pragma once

#include <hal/interfaces/SerialCommunication.hpp>
#include "infra/util/Function.hpp"
#include <string_view>
#include "../utils/CriticalSectionGuard.hpp"

class ISerialLineSource
{
public:
    virtual ~ISerialLineSource() = default;
    virtual void ReceiveLine(infra::Function<void(std::string_view)> actionOnLineReceived) = 0;
    virtual void ReceiveByte(infra::Function<void(uint8_t)> actionOnByteReceived) = 0;
};

template<std::size_t BufferSize>
class SerialLineSource : public ISerialLineSource
{
public:
    explicit SerialLineSource(hal::SerialCommunication& serial)
    {
        serial.ReceiveData([this](infra::ConstByteRange data) {
            DataReceived(data);
        });
    }

    void ReceiveLine(infra::Function<void(std::string_view)> actionOnLineReceived) override
    {
        this->actionOnLineReceived = actionOnLineReceived;
    }

    void ReceiveByte(infra::Function<void(uint8_t)> actionOnByteReceived) override
    {
        this->actionOnByteReceived = actionOnByteReceived;
    }

    void DataReceived(infra::ConstByteRange data)
    {
        for (char c : data)
        {
            ProcessCharacter(c);
        }
    }
private:
    void PushCharacter(char c)
    {
        if (inputBuffer.size() < BufferSize)
            inputBuffer.push_back(c);
    }

    void HandleLineReceived()
    {
        if (!inputBuffer.empty()) {
            infra::EventDispatcher::Instance().Schedule([this]() {
                std::string_view line;
                infra::BoundedString::iterator commandEnd = 0;
                {
                    CriticalSectionGuard guard;
                    auto commandBegin = inputBuffer.begin();
                    while (commandBegin != inputBuffer.end() && (*commandBegin == '\n' || *commandBegin == '\r')) {
                        ++commandBegin;
                    }
                    commandEnd = std::find_if(commandBegin, inputBuffer.end(), [](char c) { return c == '\n' || c == '\r'; });
                    line = std::string_view(commandBegin, std::distance(commandBegin, commandEnd));
                }
                if (actionOnLineReceived) {
                    actionOnLineReceived(line);
                }
                {
                    CriticalSectionGuard guard;
                    characterHandlerPosition -= std::distance(inputBuffer.begin(), commandEnd);
                    characterHandlerPosition = std::clamp(characterHandlerPosition, 0u, inputBuffer.size());
                    inputBuffer.erase(inputBuffer.begin(), commandEnd);
                }
            });
        } else {
            inputBuffer.clear();
            characterHandlerPosition = 0;
        }
    }

    void ProcessCharacter(char c)
    {
        PushCharacter(c);

        if (!characterHandlerActive) {
            characterHandlerActive = true;
            infra::EventDispatcher::Instance().Schedule([this]() {
                std::size_t bufferSize;
                {
                    CriticalSectionGuard guard;
                    bufferSize = inputBuffer.size();
                }
                while (characterHandlerPosition < bufferSize) {
                    const char c = inputBuffer[characterHandlerPosition++];
                    if (actionOnByteReceived) {
                        actionOnByteReceived(static_cast<uint8_t>(c));
                    }
                }
                characterHandlerActive = false;
            });
        }

        if (c == '\n' || c == '\r')
        {
            HandleLineReceived();
        }
    }

private:
    infra::BoundedString::WithStorage<BufferSize> inputBuffer;
    infra::Function<void(std::string_view)> actionOnLineReceived;
    infra::Function<void(uint8_t)> actionOnByteReceived;
    std::size_t characterHandlerPosition = 0;
    bool characterHandlerActive = false;
};
