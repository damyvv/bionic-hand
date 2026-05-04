#pragma once

#include "infra/stream/AtomicByteQueue.hpp"
#include <string_view>
#include <hal/interfaces/SerialCommunication.hpp>
#include <infra/util/BoundedDeque.hpp>

class ISerialOutput
{
public:
    virtual ~ISerialOutput() = default;
    virtual bool Write(std::string_view data) = 0;
    virtual bool Write(uint8_t byte) = 0;
};

template<std::size_t BufferSize>
class SerialOutput : public ISerialOutput
{
public:
    explicit SerialOutput(hal::SerialCommunication& serial) : serial(serial) {}
    ~SerialOutput() = default;

    bool Write(std::string_view data) override
    {
        if (writeQueue.size() + data.size() > writeQueue.max_size()) {
            return false;
        }
        for (char c : data) {
            writeQueue.push_back(static_cast<uint8_t>(c));
        }
        ProcessWriteQueue();
        return true;
    }

    bool Write(uint8_t byte) override
    {
        if (writeQueue.full()) {
            return false;
        }
        writeQueue.push_back(byte);
        ProcessWriteQueue();
        return true;
    }
private:
    void ProcessWriteQueue()
    {
        if (bytesWriting > 0 || writeQueue.empty()) {
            return;
        }

        auto bytesToWrite = writeQueue.contiguous_range(writeQueue.cbegin());
        bytesWriting = bytesToWrite.size();
        serial.SendData(bytesToWrite, [this]()
            {
                writeQueue.erase(writeQueue.cbegin(), writeQueue.cbegin() + bytesWriting);
                bytesWriting = 0;
                ProcessWriteQueue();
            });
    }

private:
    hal::SerialCommunication& serial;
    infra::BoundedDeque<uint8_t>::WithMaxSize<BufferSize> writeQueue;
    std::size_t bytesWriting = 0;
};
