#pragma once

#include <Magic/MagicItem.h>

struct BGSPerk;

struct SpellItem : MagicItem
{
    uint8_t padMI[0x108 - sizeof(MagicItem)];
    int32_t iCostOverride;
    uint32_t iFlags;
    MagicSystem::SpellType eSpellType;
    float fChargeTime;
    MagicSystem::CastingType eCastingType;
    MagicSystem::Delivery eDelivery;
    float fCastDuration;
    float fRange;
    BGSPerk* pCastingPerk;
};

static_assert(sizeof(SpellItem) == 0x130);
