#pragma once

#include <hal/interfaces/SerialCommunication.hpp>
#include <infra/util/BoundedDeque.hpp>
#include <infra/util/BoundedVector.hpp>
#include "infra/util/MemoryRange.hpp"
#include <iterator>

constexpr std::size_t COMMAND_LINE_INPUT_BUFFER_SIZE = 64;

class CommandLine
{
public:
    CommandLine(hal::SerialCommunication& serial);
    ~CommandLine() = default;

private:
    void OnDataReceived(infra::ConstByteRange data);
    void SendNextBytes();
private:
    hal::SerialCommunication& serial;
    infra::BoundedDeque<uint8_t>::WithMaxSize<COMMAND_LINE_INPUT_BUFFER_SIZE> rawInputBuffer;
    std::size_t amountOfBytesTransmitting = 0;
};
