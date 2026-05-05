#pragma once

#include "CommandLine.hpp"
#include "Command.hpp"

class CommandLineApp
{
    static constexpr std::size_t CommandCount = 2;
    static constexpr std::size_t MaxArguments = 5;

    using CliType = CommandLine<CommandCount, MaxArguments>;

public:
    explicit CommandLineApp(ISerialLineSource& lineSource, ISerialOutput& serialOutput)
        : cli(lineSource, serialOutput)
        , helpCommand(cli)
    {
        cli.RegisterCommand(helpCommand);
        cli.RegisterCommand(echoCommand);

        // cli.Run();

        EchoInput(serialOutput, lineSource);
    }

    // Non-copyable, non-movable: members hold references to each other.
    CommandLineApp(const CommandLineApp&) = delete;
    CommandLineApp& operator=(const CommandLineApp&) = delete;

    void EchoInput(ISerialOutput& serialOutput, ISerialLineSource& lineSource)
    {
        lineSource.ReceiveByte([&serialOutput](uint8_t byte) {
            serialOutput.Write(byte);
        });
    }

    CliType& Cli() { return cli; }
    const CliType& Cli() const { return cli; }

private:
    CliType cli;

    HelpCommand helpCommand;
    EchoCommand echoCommand;
};
