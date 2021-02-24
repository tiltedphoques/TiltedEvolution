#pragma once

#include <Games/Primitives.h>

struct TESQuest;

struct FormManager
{
    static FormManager* Get();

    char pad0[2024];
    GameArray<TESQuest*> quests;
};

static_assert(offsetof(FormManager, quests) == 2024);
