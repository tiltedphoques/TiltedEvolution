#pragma once

#include <Games/ExtraData.h>

struct ExtraCharge : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::Charge;

    float fCharge{};
};

static_assert(sizeof(ExtraCharge) == 0x18);

