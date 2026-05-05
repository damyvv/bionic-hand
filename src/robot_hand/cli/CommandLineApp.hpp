#pragma once

#include "CommandLine.hpp"
#include "Commands.hpp"
#include "../demo/HandDemo.hpp"
#include "../Hand.hpp"

class CommandLineApp
{
    static constexpr std::size_t CommandCount = 4;
    static constexpr std::size_t MaxArguments = 5;

    using CliType = CommandLine<CommandCount, MaxArguments>;

public:
    explicit CommandLineApp(ISerialLineSource& lineSource, ISerialOutput& serialOutput, IHand& hand, HandDemo& handDemo)
        : cli(lineSource, serialOutput)
        , helpCommand(cli)
        , openHandCommand(hand)
        , closeHandCommand(hand)
        , demoCommand(handDemo)
    {
        cli.RegisterCommand(helpCommand);
        cli.RegisterCommand(openHandCommand);
        cli.RegisterCommand(closeHandCommand);
        cli.RegisterCommand(demoCommand);

        cli.Run();

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
    OpenHandCommand openHandCommand;
    CloseHandCommand closeHandCommand;
    DemoCommand demoCommand;
};
