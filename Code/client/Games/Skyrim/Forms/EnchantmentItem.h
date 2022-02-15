#pragma once

#include "MagicItem.h"

#include <Structs/Container.h>
#include <Games/Magic/MagicSystem.h>
#include "BGSListForm.h"

struct EnchantmentItem : MagicItem
{
    static EnchantmentItem* Create(const Inventory::EnchantmentData& aData) noexcept;

    void Init(const Inventory::EnchantmentData& aData);

    int32_t iCostOverride;
    int32_t iFlags;
    MagicSystem::CastingType eCastingType;
    int32_t iChargeOverride;
    MagicSystem::Delivery eDelivery;
    MagicSystem::SpellType eSpellType;
    float fChargeTime;
    EnchantmentItem* pBaseEnchantment;
    // TODO: use BGSListForm::SaveGame() and BGSListForm::LoadGame()?
    BGSListForm* pWornRestrictions;
};

static_assert(sizeof(EnchantmentItem) == 0xC0);
