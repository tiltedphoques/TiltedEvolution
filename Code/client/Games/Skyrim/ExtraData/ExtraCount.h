#pragma once

#include <Games/ExtraData.h>

struct ExtraCount : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::Count;

    int16_t count;
};

static_assert(sizeof(ExtraCount) == 0x18);

