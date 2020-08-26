#pragma once

#ifdef TP_SKYRIM

#include <Games/Primitives.h>

struct TESQuest;

struct FormManager
{
    static FormManager* Get();

    char pad0[1864];
    GameArray<TESQuest*> quests;
};

static_assert(offsetof(FormManager, quests) == 0x748);

#endif
