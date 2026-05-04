#include "CommandLine.hpp"
#include <cassert>

CommandLine::CommandLine(hal::SerialCommunication& serial)
    : bufferedSerial(serial)
{
    Attach(bufferedSerial);
}

void CommandLine::DataReceived()
{
    auto& reader = bufferedSerial.Reader();

    while (!reader.Empty())
    {
        const auto data = reader.ExtractContiguousRange(reader.Available());
        OnDataReceived(data);
    }

    bufferedSerial.AckReceived();
}

void CommandLine::OnDataReceived(infra::ConstByteRange data)
{
}
