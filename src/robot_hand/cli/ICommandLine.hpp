#pragma once

#include "ICommand.hpp"
#include <string_view>
#include <optional>
#include <functional>

class ICommandLine
{
public:
    virtual ~ICommandLine() = default;
    virtual bool RegisterCommand(ICommand& command) = 0;
    virtual std::optional<std::reference_wrapper<ICommand>> GetCommand(std::string_view name) const = 0;
    virtual std::optional<std::reference_wrapper<ICommand>> GetCommand(size_t index) const = 0;
};
