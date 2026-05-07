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

    void EmitLine(std::string_view line)
    {
        if (actionOnLineReceived)
            actionOnLineReceived(line);
    }

    bool HasLineHandler() const
    {
        return static_cast<bool>(actionOnLineReceived);
    }

private:
    infra::Function<void(std::string_view)> actionOnLineReceived;
};
