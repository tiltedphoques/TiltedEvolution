#pragma once

#include <Forms/TESBoundAnimObject.h>
#include <Components/BGSModelMaterialSwap.h>
#include <Components/TESLeveledList.h>

struct TESLevCharacter : TESBoundAnimObject
{
    // Components
    TESLeveledList leveledList;
    BGSModelMaterialSwap modelTextureSwap;
};

static_assert(offsetof(TESLevCharacter, leveledList) == 0x68);
static_assert(offsetof(TESLevCharacter, modelTextureSwap) == 0x98);
static_assert(sizeof(TESLevCharacter) == 0xD8);
