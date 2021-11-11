#pragma once

#include <Games/ExtraData.h>

struct ExtraCount : BSExtraData
{
    int16_t count;
};

static_assert(sizeof(ExtraCount) == 0x18);

