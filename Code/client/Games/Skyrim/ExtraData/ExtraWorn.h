#pragma once

#include <Games/ExtraData.h>

struct ExtraWorn : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::Worn;
};

static_assert(sizeof(ExtraWorn) == 0x10);

