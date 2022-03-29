#pragma once

#include <Games/ExtraData.h>

struct ExtraOutfitItem : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::OutfitItem;

    TESForm* pOutfit;
};

static_assert(sizeof(ExtraOutfitItem) == 0x18);

