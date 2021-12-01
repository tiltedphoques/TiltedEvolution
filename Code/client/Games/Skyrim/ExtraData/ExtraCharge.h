#pragma once

#include <Games/ExtraData.h>

struct ExtraCharge : BSExtraData
{
    float fCharge;
};

static_assert(sizeof(ExtraCharge) == 0x18);

