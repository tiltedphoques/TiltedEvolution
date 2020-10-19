#pragma once

#include <Components/TESModel.h>

struct BGSModelMaterialSwap : TESModel
{
    virtual ~BGSModelMaterialSwap();

    uint8_t pad8[0x20 - 0x8];
};

static_assert(sizeof(BGSModelMaterialSwap) == 0x20);

