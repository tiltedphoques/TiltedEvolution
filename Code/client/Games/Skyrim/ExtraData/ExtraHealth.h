#pragma once

#include <Games/ExtraData.h>

struct ExtraHealth : BSExtraData
{
    float fHealth;
};

static_assert(sizeof(ExtraHealth) == 0x18);

