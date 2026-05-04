#pragma once

#include <hal/interfaces/SerialCommunication.hpp>
#include "infra/util/Function.hpp"
#include <string_view>

class ISerialLineSource
{
public:
    virtual ~ISerialLineSource() = default;
    virtual void ReceiveLine(infra::Function<void(std::string_view)> actionOnLineReceived) = 0;
    virtual void ReceiveByte(infra::Function<void(uint8_t)> actionOnByteReceived) = 0;
};

template<std::size_t BufferSize>
class SerialLineSource : public ISerialLineSource, private hal::BufferedSerialCommunicationObserver
{
public:
    explicit SerialLineSource(hal::SerialCommunication& serial)
        : bufferedSerial(serial)
    {
        Attach(bufferedSerial);
    }

    void ReceiveLine(infra::Function<void(std::string_view)> actionOnLineReceived) override
    {
        this->actionOnLineReceived = actionOnLineReceived;
    }

    void ReceiveByte(infra::Function<void(uint8_t)> actionOnByteReceived) override
    {
        this->actionOnByteReceived = actionOnByteReceived;
    }

    void DataReceived() override
    {
        auto& reader = bufferedSerial.Reader();

        while (!reader.Empty())
        {
            const auto data = reader.ExtractContiguousRange(reader.Available());
            for (char c : data)
            {
                if (actionOnByteReceived)
                    actionOnByteReceived(static_cast<uint8_t>(c));
                ProcessCharacter(c);
            }
        }

        bufferedSerial.AckReceived();
    }
private:
    void PushCharacter(char c)
    {
        if (inputBuffer.size() < BufferSize)
            inputBuffer.push_back(c);
    }

    void HandleLineReceived()
    {
        if (!inputBuffer.empty() && actionOnLineReceived) {
            actionOnLineReceived(std::string_view(inputBuffer.data(), inputBuffer.size()));
        }
        inputBuffer.clear();
    }

    void ProcessCharacter(char c)
    {
        if (c == '\n' || c == '\r')
        {
            HandleLineReceived();
        }
        else
        {
            PushCharacter(c);
        }
    }

private:
    constexpr static std::size_t SERIAL_INPUT_BUFFER_SIZE = 8;
    hal::BufferedSerialCommunicationOnUnbuffered::WithStorage<SERIAL_INPUT_BUFFER_SIZE> bufferedSerial;
    infra::BoundedString::WithStorage<BufferSize> inputBuffer;
    infra::Function<void(std::string_view)> actionOnLineReceived;
    infra::Function<void(uint8_t)> actionOnByteReceived;
};
