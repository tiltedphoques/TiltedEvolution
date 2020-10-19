#pragma once

#include <Forms/TESBoundAnimObject.h>
#include <Components/TESModelTextureSwap.h>
#include <Components/TESLeveledList.h>

struct TESLevCharacter : TESBoundAnimObject
{
    // Components
    TESLeveledList leveledList;
    TESModelTextureSwap modelTextureSwap;
};

static_assert(offsetof(TESLevCharacter, leveledList) == 0x30);
static_assert(offsetof(TESLevCharacter, modelTextureSwap) == 0x58);
static_assert(sizeof(TESLevCharacter) == 0x90);
