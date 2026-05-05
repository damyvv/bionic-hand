#pragma once

#include "ICommandLine.hpp"

#include "SerialLineSource.hpp"
#include "SerialOutput.hpp"
#include <infra/util/BoundedVector.hpp>

template<std::size_t MaxCommands, std::size_t MaxArguments>
class CommandLine : public ICommandLine
{
public:
    explicit CommandLine(ISerialLineSource& lineSource, ISerialOutput& serialOutput)
        : lineSource(lineSource)
        , serialOutput(serialOutput)
    {
    }
    
    void Run() {
        lineSource.ReceiveLine([this](std::string_view line) { ProcessLine(line); });
    }

    bool RegisterCommand(ICommand& command) override {
        if (commands.full())
            return false;

        commands.push_back(command);
        return true;
    }

    std::optional<std::reference_wrapper<ICommand>> GetCommand(std::string_view name) const override {
        for (const auto& command : commands)
        {
            if (command.get().Name() == name)
                return command;
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<ICommand>> GetCommand(size_t index) const override {
        if (index >= commands.size())
            return std::nullopt;

        return commands[index];
    }

private:
    void ProcessLine(std::string_view line) {
        if (line.empty())
            return;
        auto commandName = line.substr(0, line.find(' '));
        auto command = GetCommand(commandName);
        if (!command.has_value())
        {
            serialOutput.Write("Unknown command: ");
            serialOutput.Write(commandName);
            serialOutput.Write("\n");
            return;
        }
        // Split the arguments by the space character, but only up to MaxArguments arguments, fail if there are more.
        infra::BoundedVector<std::string_view>::WithMaxSize<MaxArguments> arguments;
        size_t start = commandName.size();
        while (arguments.size() < MaxArguments && start < line.size())
        {
            // Skip leading spaces
            while (start < line.size() && line[start] == ' ')
                ++start;

            if (start >= line.size())
                break;

            size_t end = line.find(' ', start);
            if (end == std::string_view::npos)
                end = line.size();

            arguments.push_back(line.substr(start, end - start));
            start = end;
        }
        if (start < line.size())
        {
            serialOutput.Write("Too many arguments");
            return;
        }
        command.value().get().Execute(arguments.range(), serialOutput);
    }

private:
    ISerialLineSource& lineSource;
    ISerialOutput& serialOutput;
    infra::BoundedVector<std::reference_wrapper<ICommand>>::WithMaxSize<MaxCommands> commands;
};
