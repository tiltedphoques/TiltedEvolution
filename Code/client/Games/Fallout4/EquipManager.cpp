#include <EquipManager.h>

#include <Actor.h>

namespace
{
// BGSObjectInstance
struct ObjectInstance
{
    TESForm* object;
    void* instanceData;
};
} // namespace

EquipManager* EquipManager::Get() noexcept
{
    static VersionDbPtr<EquipManager*> s_singleton(4798287);
    return *s_singleton.Get();
}

void* EquipManager::Equip(Actor* apActor, TESForm* apItem, ExtraDataList*, int aCount, TESForm* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySound, bool abApplyNow)
{
    TP_THIS_FUNCTION(TEquipObject, bool, EquipManager, Actor*, const ObjectInstance&, uint32_t, uint32_t, TESForm*, bool, bool, bool, bool, bool);
    static VersionDbPtr<TEquipObject> equipObject(2231392);

    if (!apActor || !apItem)
        return nullptr;

    const ObjectInstance instance{apItem, nullptr};
    TiltedPhoques::ThisCall(equipObject, this, apActor, instance, 0u, static_cast<uint32_t>(aCount), apSlot, abQueueEquip, abForceEquip, abPlaySound, abApplyNow, false);
    return nullptr;
}

void* EquipManager::UnEquip(Actor* apActor, TESForm* apItem, ExtraDataList*, int aCount, TESForm* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySound, bool abApplyNow, TESForm* apSlotToReplace)
{
    TP_THIS_FUNCTION(TUnequipObject, bool, EquipManager, Actor*, const ObjectInstance*, uint32_t, TESForm*, uint32_t, bool, bool, bool, bool, TESForm*);
    static VersionDbPtr<TUnequipObject> unequipObject(2231395);

    if (!apActor || !apItem)
        return nullptr;

    const ObjectInstance instance{apItem, nullptr};
    TiltedPhoques::ThisCall(unequipObject, this, apActor, &instance, static_cast<uint32_t>(aCount), apSlot, 0u, abQueueEquip, abForceEquip, abPlaySound, abApplyNow, apSlotToReplace);
    return nullptr;
}

// Fallout 4 has no equippable spells or shouts.
void* EquipManager::EquipSpell(Actor*, TESForm*, uint32_t)
{
    return nullptr;
}

void* EquipManager::UnEquipSpell(Actor*, TESForm*, uint32_t)
{
    return nullptr;
}

void* EquipManager::EquipShout(Actor*, TESForm*)
{
    return nullptr;
}

void* EquipManager::UnEquipShout(Actor*, TESForm*)
{
    return nullptr;
}

void EquipManager::UnequipAll(Actor* apActor)
{
    if (apActor)
        apActor->UnEquipAll();
}
