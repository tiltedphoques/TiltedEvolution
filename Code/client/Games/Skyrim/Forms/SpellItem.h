#pragma once

#include <Forms/MagicItem.h>

#include <Components/BGSEquipType.h>
#include <Components/BGSMenuDisplayObject.h>
#include <Components/TESDescription.h>

struct SpellItem : MagicItem
{
    BGSEquipType equipType;
    BGSMenuDisplayObject menuDisplayObject;
    TESDescription description;
    uint32_t unk6C[5];
    float castTime;
    MagicSystem::CastingType eCastingType;
    MagicSystem::Delivery eDelivery;
    // more stuff
};

