#include "CommandLine.hpp"
#include "../utils/CriticalSectionGuard.hpp"

CommandLine::CommandLine(hal::SerialCommunication& serial)
    : serial(serial)
{
    serial.ReceiveData([this](infra::ConstByteRange data)
        {
            OnDataReceived(data);
        });
}

void CommandLine::OnDataReceived(infra::ConstByteRange data)
{
    if (rawInputBuffer.size() >= rawInputBuffer.max_size()) {
        // If the new data exceeds the buffer capacity, ignore the new data.
        assert(false);
        return;
    }
    rawInputBuffer.insert(rawInputBuffer.end(), data.begin(), data.end());
    SendNextBytes();
}

void CommandLine::SendNextBytes()
{
    if (amountOfBytesTransmitting > 0 || rawInputBuffer.empty()) {
        return;
    }
    const auto toTransmit = rawInputBuffer.contiguous_range(rawInputBuffer.begin());
    amountOfBytesTransmitting = toTransmit.size();
    serial.SendData(toTransmit, [this]()
        {
            CriticalSectionGuard guard;
            rawInputBuffer.erase(rawInputBuffer.begin(), rawInputBuffer.begin() + amountOfBytesTransmitting);
            amountOfBytesTransmitting = 0;
            SendNextBytes();
        });
}
