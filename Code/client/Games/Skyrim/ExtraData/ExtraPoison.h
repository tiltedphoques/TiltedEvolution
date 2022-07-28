#pragma once

#include "ExtraData.h"

struct AlchemyItem;

struct ExtraPoison : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::Poison;

    AlchemyItem* pPoison{};
    uint32_t uiCount{};
};

static_assert(sizeof(ExtraPoison) == 0x20);

