#pragma once

#include "TESLeveledList.h"

struct TESLevItem : TESBoundObject, TESLeveledList
{
};

static_assert(sizeof(TESLevItem) == 0x58);
