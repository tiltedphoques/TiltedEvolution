#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace GoldLoss
{
inline bool IsValid(float aSetting) noexcept
{
    return std::isfinite(aSetting) && aSetting >= 0.0f;
}

inline int32_t Calculate(int32_t aGoldCount, float aSetting) noexcept
{
    if (aGoldCount <= 0 || !IsValid(aSetting))
        return 0;

    // 0..1 is a fraction; larger values are a flat fee. Clamp before converting
    // to an integer so even huge finite settings cannot overflow the deduction.
    // Preserve the existing float multiplication for fractional penalties.
    const double cAmount = aSetting <= 1.0f ? static_cast<double>(aGoldCount * aSetting) : static_cast<double>(aSetting);
    return static_cast<int32_t>(std::min(cAmount, static_cast<double>(aGoldCount)));
}
}
