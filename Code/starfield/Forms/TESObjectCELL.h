#pragma once

#include "TESHandleForm.h"

struct TESObjectCELL : TESHandleForm
{
    bool IsInteriorCell() { return cellFlags & 1; }

    uint8_t unkTESObjectCELL[0x48 - sizeof(TESHandleForm)];
    uint32_t cellFlags;
    uint8_t unk4C[0x150 - 0x4C];
};

static_assert(sizeof(TESObjectCELL) == 0x150);
