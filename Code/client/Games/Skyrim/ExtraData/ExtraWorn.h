#pragma once

#include <Games/ExtraData.h>

struct ExtraWorn : BSExtraData
{
};

static_assert(sizeof(ExtraWorn) == 0x10);

