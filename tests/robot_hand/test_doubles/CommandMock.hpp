#pragma once

#include "cli/ICommand.hpp"
#include <gmock/gmock.h>

class CommandMock
    : public ICommand
{
public:
    MOCK_METHOD(void, Execute, (infra::MemoryRange<const std::string_view> arguments, ISerialOutput& serialOutput), (override));
    MOCK_METHOD(std::string_view, Name, (), (const, override));
    MOCK_METHOD(std::string_view, Description, (), (const, override));
};
