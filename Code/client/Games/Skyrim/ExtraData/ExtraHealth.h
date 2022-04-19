#pragma once

#include <Games/ExtraData.h>

struct ExtraHealth : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::Health;

    float fHealth{};
};

static_assert(sizeof(ExtraHealth) == 0x18);

