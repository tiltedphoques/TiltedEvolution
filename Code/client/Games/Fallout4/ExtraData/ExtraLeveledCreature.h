#pragma once

#include "ExtraData.h"

struct TESActorBase;

struct ExtraLeveledCreature : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::LeveledCreature;

    TESActorBase* originalBase;      // 18
    TESActorBase* templateBases[13]; // 20 one per template use flag
};

static_assert(offsetof(ExtraLeveledCreature, originalBase) == 0x18);
static_assert(sizeof(ExtraLeveledCreature) == 0x88);
