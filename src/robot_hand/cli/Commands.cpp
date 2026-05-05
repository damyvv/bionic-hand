#include "Commands.hpp"
#include <charconv>

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
        uint8_t fingerId = 0;
        const auto result = std::from_chars(argument.data(), argument.data() + argument.size(), fingerId);
        
        if (result.ec != std::errc() || fingerId >= hand.GetFingerCount())
        {
            serialOutput.Write("Invalid argument, expected a number between 0 and ");
            serialOutput.Write(std::to_string(hand.GetFingerCount() - 1));
            serialOutput.Write("\n");
            return;
        }
        
        hand.OpenFinger(fingerId);
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
        uint8_t fingerId = 0;
        const auto result = std::from_chars(argument.data(), argument.data() + argument.size(), fingerId);
        
        if (result.ec != std::errc() || fingerId >= hand.GetFingerCount())
        {
            serialOutput.Write("Invalid argument, expected a number between 0 and ");
            serialOutput.Write(std::to_string(hand.GetFingerCount() - 1));
            serialOutput.Write("\n");
            return;
        }
        
        hand.CloseFinger(fingerId);
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
