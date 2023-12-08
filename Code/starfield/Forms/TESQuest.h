#pragma once

#include "BGSStoryManagerTreeForm.h"
#include "TESFullName.h"

struct TESQuest : BGSStoryManagerTreeForm, TESFullName
{
    uint8_t unkTESQuest[0x348 - sizeof(BGSStoryManagerTreeForm)];
};

static_assert(sizeof(TESQuest) == 0x358);
