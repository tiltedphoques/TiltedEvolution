#pragma once

#include "TESForm.h"

struct TESPackage : TESForm
{
    uint8_t unk20[0xD8 - 0x20];
    uint32_t ePROCEDURE_TYPE;
    uint32_t uiRefCount;
};

static_assert(sizeof(TESPackage) == 0xE0);
