#pragma once

#include <string_view>

class ICommand
{
public:
    virtual ~ICommand() = default;
    virtual void Execute() = 0;
    virtual std::string_view Name() const = 0;
    virtual std::string_view Description() const = 0;
};
