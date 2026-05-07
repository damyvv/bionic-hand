#pragma once

#include "ICommand.hpp"
#include "ICommandLine.hpp"
#include "SerialOutput.hpp"
#include "../Hand.hpp"
#include "../demo/HandDemo.hpp"

#include <cstdio>

class HelpCommand : public ICommand
{
public:
    explicit HelpCommand(ICommandLine& commandLine);
    void Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput) override;
    std::string_view Name() const override;
    std::string_view Description() const override;
private:
    ICommandLine& commandLine;
};

class OpenHandCommand : public ICommand
{
public:
    explicit OpenHandCommand(IHand& hand);
    void Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput) override;
    std::string_view Name() const override;
    std::string_view Description() const override;
private:
    IHand& hand;
};

class CloseHandCommand : public ICommand
{
public:
    explicit CloseHandCommand(IHand& hand);
    void Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput) override;
    std::string_view Name() const override;
    std::string_view Description() const override;
private:
    IHand& hand;
};

class DemoCommand : public ICommand
{
public:
    explicit DemoCommand(HandDemo& handDemo);
    void Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput) override;
    std::string_view Name() const override;
    std::string_view Description() const override;
private:
    HandDemo& handDemo;
};

class WaveCommand : public ICommand
{
public:
    explicit WaveCommand(HandDemo& handDemo);
    void Execute(infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput) override;
    std::string_view Name() const override;
    std::string_view Description() const override;
private:
    HandDemo& handDemo;
    int counter = 0;
    
};
