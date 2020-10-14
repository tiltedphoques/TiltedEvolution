#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/TESModel.h>

struct BGSModelMaterialSwap : TESModel
{
    virtual ~BGSModelMaterialSwap();

    uint8_t pad8[0x20 - 0x8];
};

static_assert(sizeof(BGSModelMaterialSwap) == 0x20);


#endif
