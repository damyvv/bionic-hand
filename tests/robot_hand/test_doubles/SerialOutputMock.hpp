#pragma once

#include "cli/SerialOutput.hpp"
#include <gmock/gmock.h>

class SerialOutputMock
    : public ISerialOutput
{
public:
    MOCK_METHOD(bool, Write, (std::string_view data), (override));
    MOCK_METHOD(bool, Write, (uint8_t byte), (override));
};
