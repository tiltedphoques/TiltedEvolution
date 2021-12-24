#pragma once

#include <Games/ExtraData.h>

struct AlchemyItem;

struct ExtraPoison : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::Poison;

    AlchemyItem* pPoison{};
    uint32_t uiCount{};
};

static_assert(sizeof(ExtraPoison) == 0x20);

