#include "CriticalSectionGuard.hpp"

#include DEVICE_HEADER

CriticalSectionGuard::CriticalSectionGuard()
{
    __disable_irq();
    __DSB();
}

CriticalSectionGuard::~CriticalSectionGuard()
{
    __DSB();
    __enable_irq();
}
