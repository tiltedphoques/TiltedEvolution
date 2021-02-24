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
    uint32_t unk6C[3];
    float castTime;
    uint32_t castType;
    // more stuff
};
