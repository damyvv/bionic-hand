#pragma once

#include "ICommand.hpp"
#include "ICommandLine.hpp"
#include "SerialOutput.hpp"

#include <cstdio>

class HelpCommand : public ICommand
{
public:
    explicit HelpCommand(ICommandLine& commandLine) : commandLine(commandLine) {}
    void Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput) override {
        if (!arguments.empty())
        {
            serialOutput.Write("This command does not take any arguments\n");
            return;
        }
        for (std::size_t i = 0;; ++i)
        {
            const auto optionalCommand = commandLine.GetCommand(i);
            if (!optionalCommand.has_value()) break;
            const auto& command = optionalCommand.value().get();

            const std::string_view name = command.Name();
            const std::string_view description = command.Description();
            serialOutput.Write(name);
            serialOutput.Write(": ");
            serialOutput.Write(description);
            serialOutput.Write("\n");
        }
    }
    std::string_view Name() const override { return "help"; }
    std::string_view Description() const override { return "Lists all available commands"; }

private:
    ICommandLine& commandLine;
};

class EchoCommand : public ICommand
{
public:
    void Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput) override {
        for (const auto argument : arguments)
        {
            serialOutput.Write(argument);
            serialOutput.Write(" ");
        }
        serialOutput.Write("\n");
    }
    std::string_view Name() const override { return "echo"; }
    std::string_view Description() const override { return "Echoes the provided arguments"; }
};
