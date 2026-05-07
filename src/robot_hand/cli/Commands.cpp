#include "Commands.hpp"
#include <charconv>
#include "../demo/DemoStates.hpp"

namespace {
    std::optional<std::size_t> StringToValue(std::string_view string)
    {
        std::size_t value = 0;
        const auto result = std::from_chars(string.data(), string.data() + string.size(), value);
        if (result.ec != std::errc())
        {
            return std::nullopt;
        }
        return value;
    }
}

HelpCommand::HelpCommand(ICommandLine& commandLine)
    : commandLine(commandLine)
{
}

void HelpCommand::Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput) {
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

std::string_view HelpCommand::Name() const
{
    return "help";
}

std::string_view HelpCommand::Description() const
{
    return "Lists all available commands with their description";
}

OpenHandCommand::OpenHandCommand(IHand& hand)
    : hand(hand)
{
}

void OpenHandCommand::Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput)
{
    if (arguments.empty())
    {
        hand.OpenFingers();
    }
    else if (arguments.size() == 1)
    {
        const std::string_view argument = arguments.front();
        const auto optionalValue = StringToValue(argument);
        if (!optionalValue.has_value() || optionalValue.value() >= hand.GetFingerCount())
        {
            serialOutput.Write("Invalid argument, expected a number between 0 and ");
            serialOutput.Write(std::to_string(hand.GetFingerCount() - 1));
            serialOutput.Write("\n");
            return;
        }
        
        hand.OpenFinger(optionalValue.value());
    }
    else
    {
        serialOutput.Write("Too many arguments, expected at most 1\n");
        return;
    }
    serialOutput.Write("Opening...\n");
}

std::string_view OpenHandCommand::Name() const
{
    return "open";
}

std::string_view OpenHandCommand::Description() const
{
    return "Opens the hand";
}

CloseHandCommand::CloseHandCommand(IHand& hand)
    : hand(hand)
{
}

void CloseHandCommand::Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput)
{
    if (arguments.empty())
    {
        hand.CloseFingers();
    }
    else if (arguments.size() == 1)
    {
        const std::string_view argument = arguments.front();
        const auto optionalValue = StringToValue(argument);
        if (!optionalValue.has_value() || optionalValue.value() >= hand.GetFingerCount())
        {
            serialOutput.Write("Invalid argument, expected a number between 0 and ");
            serialOutput.Write(std::to_string(hand.GetFingerCount() - 1));
            serialOutput.Write("\n");
            return;
        }
        hand.CloseFinger(optionalValue.value());
    }
    else
    {
        serialOutput.Write("Too many arguments, expected at most 1\n");
        return;
    }
    serialOutput.Write("Closing...\n");
}

std::string_view CloseHandCommand::Name() const
{
    return "close";
}

std::string_view CloseHandCommand::Description() const
{
    return "Closes the hand";
}

DemoCommand::DemoCommand(HandDemo& handDemo)
    : handDemo(handDemo)
{
}

void DemoCommand::Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput)
{
    if (arguments.size() != 1)
    {
        serialOutput.Write("This command expects exactly 1 argument\n");
        return;
    }
    if (arguments.front() == "start")
    {
        handDemo.StartDemo();
        serialOutput.Write("Starting demo...\n");
    }
    else if (arguments.front() == "stop")
    {
        handDemo.StopDemo();
        serialOutput.Write("Stopping demo...\n");
    }
    else
    {
        serialOutput.Write("Invalid argument, expected 'start' or 'stop'\n");
    }
}

std::string_view DemoCommand::Name() const
{
    return "demo";
}

std::string_view DemoCommand::Description() const
{
    return "Starts or stops the demo (argument should be 'start' or 'stop')";
}

WaveCommand::WaveCommand(HandDemo& handDemo)
    : handDemo(handDemo)
{
}

void WaveCommand::Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput)
{
    int waveCount = 0;
    if (arguments.empty()) {
        waveCount = 2; // Default wave count
    } else if (arguments.size() == 1) {
        const std::string_view argument = arguments.front();
        const auto optionalValue = StringToValue(argument);
        if (!optionalValue.has_value())
        {
            serialOutput.Write("Invalid argument, expected a number\n");
            return;
        }
        waveCount = optionalValue.value();
    } else {
        serialOutput.Write("Too many arguments, expected at most 1\n");
        return;
    }
    auto& waveState = WaveState::GetInstance();
    waveState.SetWaveCount(waveCount);
    waveState.SetNextState(&IdleState::GetInstance());
    handDemo.TransitionToState(&waveState);
    serialOutput.Write("Waving...\n");
}

std::string_view WaveCommand::Name() const
{
    return "wave";
}

std::string_view WaveCommand::Description() const
{
    return "Waves the hand (optionally takes the number of waves as an argument)";
}
