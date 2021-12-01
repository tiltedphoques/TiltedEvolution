#pragma once

#include <Games/ExtraData.h>

struct ExtraWornLeft : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::WornLeft;
};

static_assert(sizeof(ExtraWornLeft) == 0x10);

