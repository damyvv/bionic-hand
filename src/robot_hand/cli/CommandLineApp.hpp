#pragma once

#include "CommandLine.hpp"
#include "Commands.hpp"
#include "../demo/HandDemo.hpp"
#include "../Hand.hpp"

class CommandLineApp
{
    static constexpr std::size_t CommandCount = 6;
    static constexpr std::size_t MaxArguments = 5;

    using CliType = CommandLine<CommandCount, MaxArguments>;

public:
    explicit CommandLineApp(ISerialLineSource& lineSource, ISerialOutput& serialOutput, IHand& hand, HandDemo& handDemo)
        : cli(lineSource, serialOutput)
        , helpCommand(cli)
        , openHandCommand(hand)
        , closeHandCommand(hand)
        , demoCommand(handDemo)
        , waveCommand(handDemo)
        , gameCommand(handDemo)
    {
        cli.RegisterCommand(helpCommand);
        cli.RegisterCommand(openHandCommand);
        cli.RegisterCommand(closeHandCommand);
        cli.RegisterCommand(demoCommand);
        cli.RegisterCommand(waveCommand);
        cli.RegisterCommand(gameCommand);

        cli.Run();
    }

    // Non-copyable, non-movable: members hold references to each other.
    CommandLineApp(const CommandLineApp&) = delete;
    CommandLineApp& operator=(const CommandLineApp&) = delete;

    CliType& Cli() { return cli; }
    const CliType& Cli() const { return cli; }

private:
    CliType cli;

    HelpCommand helpCommand;
    OpenHandCommand openHandCommand;
    CloseHandCommand closeHandCommand;
    DemoCommand demoCommand;
    WaveCommand waveCommand;
    GameCommand gameCommand;
};
