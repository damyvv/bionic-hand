#pragma once

class IServo
{
public:
    virtual ~IServo() = default;

    virtual void SetAngle(float angle) = 0;
};
