#pragma once

#if TP_SKYRIM

#include <Games/ExtraData.h>

struct TESNPC;

struct ExtraLeveledCreature : BSExtraData
{
    virtual ~ExtraLeveledCreature();

    TESNPC* npc1;
    TESNPC* npc2;
};

static_assert(sizeof(ExtraLeveledCreature) == 0x20);
static_assert(offsetof(ExtraLeveledCreature, npc1) == 0x10);

#endif
