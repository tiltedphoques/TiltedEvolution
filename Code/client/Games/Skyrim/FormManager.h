#pragma once

#include <Games/Primitives.h>

struct TESQuest;

// Actually, TESDataHandler
struct FormManager
{
    static FormManager* Get();

    char pad0[1864];
    GameArray<TESQuest*> quests;
};

static_assert(offsetof(FormManager, quests) == 0x748);

