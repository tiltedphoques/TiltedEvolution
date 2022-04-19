#pragma once

#include <Forms/MagicItem.h>

#include <Components/BGSEquipType.h>
#include <Components/BGSMenuDisplayObject.h>
#include <Components/TESDescription.h>

struct SpellItem : MagicItem
{
    enum class SpellFlag
    {
        kNone = 0,
        kCostOverride = 1 << 0,
        kFoodItem = 1 << 1,
        kExtendDuration = 1 << 3,
        kPCStartSpell = 1 << 17,
        kInstantCast = 1 << 18,
        kIgnoreLOSCheck = 1 << 19,
        kIgnoreResistance = 1 << 20,
        kNoAbsorb = 1 << 21,
        kNoDualCastMods = 1 << 23
    };

    BGSEquipType equipType;
    BGSMenuDisplayObject menuDisplayObject;
    TESDescription description;
    uint32_t unk6C[3];
    float castTime;
    MagicSystem::CastingType eCastingType;
    MagicSystem::Delivery eDelivery;
    // more stuff
};

