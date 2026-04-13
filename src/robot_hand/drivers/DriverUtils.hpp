#pragma once

#include "infra/util/ReallyAssert.hpp"
#include <cstdint>
#include <cmath>

namespace DriverUtils
{
    constexpr uint32_t HertzToMicroseconds(uint16_t frequency_hz)
    {
        really_assert(frequency_hz > 0);
        return static_cast<uint32_t>(std::round(1'000'000.0f / frequency_hz));
    }

    // Simple linear interpolation function
    template<typename A, typename T>
    constexpr A lerp(A a, A b, T t)
    {
        return a + static_cast<A>(std::round((b - a) * t));
    }
}