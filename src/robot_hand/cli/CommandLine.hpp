#pragma once

#include "SerialLineSource.hpp"
#include "SerialOutput.hpp"
#include "Command.hpp"
#include <infra/util/BoundedVector.hpp>

class ICommandLine
{
public:
    virtual ~ICommandLine() = default;
    virtual bool RegisterCommand(ICommand& command) = 0;
    virtual std::optional<std::reference_wrapper<ICommand>> GetCommand(std::string_view name) const = 0;
    virtual std::optional<std::reference_wrapper<ICommand>> GetCommand(size_t index) const = 0;
};

template<std::size_t MaxCommands>
class CommandLine : public ICommandLine
{
public:
    explicit CommandLine(ISerialLineSource& lineSource, ISerialOutput& serialOutput)
        : lineSource(lineSource), serialOutput(serialOutput) {};

    bool RegisterCommand(ICommand& command) override;

private:
    ISerialLineSource& lineSource;
    ISerialOutput& serialOutput;
    infra::BoundedVector<ICommand*>::WithMaxSize<MaxCommands> commands;
};
