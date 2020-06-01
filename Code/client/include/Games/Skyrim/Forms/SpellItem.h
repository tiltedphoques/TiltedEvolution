#pragma once


#if TP_SKYRIM

#include <Games/Skyrim/Forms/MagicItem.h>

#include <Games/Skyrim/Components/BGSEquipType.h>
#include <Games/Skyrim/Components/BGSMenuDisplayObject.h>
#include <Games/Skyrim/Components/TESDescription.h>

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

#endif
