#pragma once

#include "ExtraData.h"

enum class SOUL_LEVEL
{
    SOUL_NONE = 0x0,
    SOUL_PETTY = 0x1,
    SOUL_LESSER = 0x2,
    SOUL_COMMON = 0x3,
    SOUL_GREATER = 0x4,
    SOUL_GRAND = 0x5,
    SOUL_LEVEL_COUNT = 0x6,
};

struct ExtraSoul : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::Soul;

    SOUL_LEVEL cSoul{};
};

static_assert(sizeof(ExtraSoul) == 0x18);

