#pragma once

class CriticalSectionGuard
{
public:
    CriticalSectionGuard();
    
    CriticalSectionGuard(const CriticalSectionGuard&) = delete;
    CriticalSectionGuard& operator=(const CriticalSectionGuard&) = delete;

    ~CriticalSectionGuard();
};
