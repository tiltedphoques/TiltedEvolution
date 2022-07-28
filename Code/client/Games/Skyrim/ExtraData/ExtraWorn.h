#pragma once

#include "ExtraData.h"

struct ExtraWorn : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::Worn;
};

static_assert(sizeof(ExtraWorn) == 0x10);

