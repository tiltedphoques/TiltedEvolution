#pragma once

#include "ExtraData.h"

struct ExtraCount : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::Count;

    int16_t count;
};

static_assert(sizeof(ExtraCount) == 0x18);

