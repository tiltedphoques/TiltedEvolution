#pragma once

#include <base/external/xoshiro256ss/xoshiro256ss.h>
#include <random>

namespace base
{
// NOTE(Force): this is only 32 bytes big, so we can instance our generator on the fly.

template <typename T> T RandomIntegral(T aMin, T aMax)
{
    xoshiro256ss rng(std::random_device{}());
    return std::uniform_int_distribution<T>(aMin, aMax)(rng);
}

// for float types.
template <typename T> T RandomReal(T aMin, T aMax)
{
    xoshiro256ss rng(std::random_device{}());
    return std::uniform_real_distribution<T>(aMin, aMax)(rng);
}

} // namespace base
