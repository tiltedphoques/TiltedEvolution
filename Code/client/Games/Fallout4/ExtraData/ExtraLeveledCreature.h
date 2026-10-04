#pragma once

#include "ExtraData.h"

struct TESActorBase;

struct ExtraLeveledCreature : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::LeveledCreature;

    virtual ~ExtraLeveledCreature();

    TESActorBase* originalBase;
    TESActorBase* templateBase;
};

static_assert(sizeof(ExtraLeveledCreature) == 0x20);
static_assert(offsetof(ExtraLeveledCreature, originalBase) == 0x10);
static_assert(offsetof(ExtraLeveledCreature, templateBase) == 0x18);
