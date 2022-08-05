#include "AlchemyItem.h"

#include <Systems/ModSystem.h>
#include <Magic/EffectItem.h>
#include <World.h>
#include <Games/BGSCreatedObjectManager.h>

AlchemyItem* AlchemyItem::Create(const Inventory::EnchantmentData& aData) noexcept
{
    TP_THIS_FUNCTION(TAddPoison, void, BGSCreatedObjectManager, AlchemyItem**, GameArray<EffectItem>*);
    POINTER_SKYRIMSE(TAddPoison, addPoison, 36168);
    TP_THIS_FUNCTION(TAddPotion, void, BGSCreatedObjectManager, AlchemyItem**, GameArray<EffectItem>*);
    // TODO: confirm these IDs
    POINTER_SKYRIMSE(TAddPotion, addPotion, 36167);

    ModSystem& modSystem = World::Get().GetModSystem();

    GameArray<EffectItem> effects{};
    effects.Resize(aData.Effects.size());
    for (int i = 0; i < aData.Effects.size(); i++)
    {
        Inventory::EffectItem effect = aData.Effects[i];

        EffectItem effectItem{};
        effectItem.data.fMagnitude = effect.Magnitude;
        effectItem.data.iArea = effect.Area;
        effectItem.data.iDuration = effect.Duration;
        effectItem.fRawCost = effect.RawCost;
        effectItem.pEffectSetting =
            Cast<EffectSetting>(TESForm::GetById(modSystem.GetGameId(effect.EffectId)));
        if (!effectItem.pEffectSetting)
            spdlog::error("Effect setting not found: {:X}:{:X}", effect.EffectId.ModId, effect.EffectId.BaseId);

        // TODO: TESCondition?

        effects[i] = effectItem;
    }

    BGSCreatedObjectManager* objManager = BGSCreatedObjectManager::Get();

    AlchemyItem* item = Memory::Allocate<AlchemyItem>();
    if (aData.IsWeapon)
        ThisCall(addPoison, objManager, &item, &effects);
    else
        ThisCall(addPotion, objManager, &item, &effects);

    return item;
}
