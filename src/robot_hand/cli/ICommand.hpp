#pragma once

#include <string_view>
#include <infra/util/MemoryRange.hpp>
#include "SerialOutput.hpp"

class ICommand
{
public:
    virtual ~ICommand() = default;
    virtual void Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput) = 0;
    virtual std::string_view Name() const = 0;
    virtual std::string_view Description() const = 0;
};
