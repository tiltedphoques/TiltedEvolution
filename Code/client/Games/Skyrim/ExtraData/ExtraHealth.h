#pragma once

#include "ExtraData.h"

struct ExtraHealth : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::Health;

    float fHealth{};
};

static_assert(sizeof(ExtraHealth) == 0x18);

