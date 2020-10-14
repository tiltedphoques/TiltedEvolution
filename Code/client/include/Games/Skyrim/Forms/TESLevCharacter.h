#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESBoundAnimObject.h>
#include <Games/Skyrim/Components/TESModelTextureSwap.h>
#include <Games/Skyrim/Components/TESLeveledList.h>

struct TESLevCharacter : TESBoundAnimObject
{
    // Components
    TESLeveledList leveledList;
    TESModelTextureSwap modelTextureSwap;
};

static_assert(offsetof(TESLevCharacter, leveledList) == 0x30);
static_assert(offsetof(TESLevCharacter, modelTextureSwap) == 0x58);
static_assert(sizeof(TESLevCharacter) == 0x90);

#endif
