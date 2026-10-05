#include <EquipManager.h>

#include <Actor.h>
#include <Games/ActorExtension.h>
#include <Games/Overrides.h>
#include <Events/EquipmentChangeEvent.h>
#include <World.h>

namespace
{
// BGSObjectInstance
struct ObjectInstance
{
    TESForm* object;
    void* instanceData;
};

TP_THIS_FUNCTION(TEquipObject, bool, EquipManager, Actor*, const ObjectInstance*, uint32_t, uint32_t, TESForm*, bool, bool, bool, bool, bool);
TP_THIS_FUNCTION(TUnequipObject, bool, EquipManager, Actor*, const ObjectInstance*, uint32_t, TESForm*, uint32_t, bool, bool, bool, bool, TESForm*);

TEquipObject* RealEquipObject = nullptr;
TUnequipObject* RealUnequipObject = nullptr;

void QueueEquipmentChange(Actor* apActor, EquipmentChangeEvent aEvent)
{
    const auto ownershipToken = Utils::GetLocalOwnershipToken(apActor->formID);
    if (!ownershipToken)
        return;

    aEvent.ServerId = ownershipToken->ServerId;
    aEvent.OwnershipEpoch = ownershipToken->OwnershipEpoch;
    World::Get().GetRunner().Trigger(std::move(aEvent));
}

bool HasItem(Actor* apActor, const TESForm* apItem)
{
    if (!apActor->inventoryList)
        return false;

    for (const auto& item : apActor->inventoryList->items)
    {
        if (item.object == apItem)
            return true;
    }
    return false;
}
} // namespace

EquipManager* EquipManager::Get() noexcept
{
    static VersionDbPtr<EquipManager*> s_singleton(4798287);
    return *s_singleton.Get();
}

void* EquipManager::Equip(Actor* apActor, TESForm* apItem, ExtraDataList*, int aCount, TESForm* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySound, bool abApplyNow)
{
    if (!apActor || !apItem)
        return nullptr;

    ScopedEquipOverride equipOverride;

    // Puppets mirror their owner, who may have picked the item up since they spawned.
    if (apActor->GetExtension()->IsRemote() && !HasItem(apActor, apItem))
    {
        ExtraDataList* pExtra = nullptr;
        apActor->AddObjectToContainer(static_cast<TESBoundObject*>(apItem), &pExtra, std::max(aCount, 1), nullptr, ITEM_REMOVE_REASON::kRemove);
    }

    const ObjectInstance instance{apItem, nullptr};
    TiltedPhoques::ThisCall(RealEquipObject, this, apActor, &instance, 0u, static_cast<uint32_t>(aCount), apSlot, abQueueEquip, abForceEquip, abPlaySound, abApplyNow, false);
    return nullptr;
}

void* EquipManager::UnEquip(Actor* apActor, TESForm* apItem, ExtraDataList*, int aCount, TESForm* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySound, bool abApplyNow, TESForm* apSlotToReplace)
{
    if (!apActor || !apItem)
        return nullptr;

    ScopedEquipOverride equipOverride;

    const ObjectInstance instance{apItem, nullptr};
    TiltedPhoques::ThisCall(RealUnequipObject, this, apActor, &instance, static_cast<uint32_t>(aCount), apSlot, 0u, abQueueEquip, abForceEquip, abPlaySound, abApplyNow, apSlotToReplace);
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

bool TP_MAKE_THISCALL(HookEquipObject, EquipManager, Actor* apActor, const ObjectInstance* apInstance, uint32_t aStackId, uint32_t aCount, TESForm* apSlot,
                      bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, bool abLocked)
{
    TESForm* pItem = apInstance ? apInstance->object : nullptr;
    if (apActor && pItem)
    {
        const auto pExtension = apActor->GetExtension();
        if (pExtension->IsRemote() && !ScopedEquipOverride::IsOverriden())
            return false;

        // Consumables are equipped too; syncing them would consume them twice.
        // Queued requests are applied later without passing through here again.
        if (pExtension->IsLocal() && !pItem->IsConsumable())
        {
            EquipmentChangeEvent evt{};
            evt.ActorId = apActor->formID;
            evt.Count = aCount;
            evt.ItemId = pItem->formID;
            evt.EquipSlotId = apSlot ? apSlot->formID : 0;
            evt.IsAmmo = pItem->formType == FormType::Ammo;

            QueueEquipmentChange(apActor, std::move(evt));
        }
    }

    ScopedUnequipOverride _;

    return TiltedPhoques::ThisCall(RealEquipObject, apThis, apActor, apInstance, aStackId, aCount, apSlot, abQueueEquip, abForceEquip, abPlaySounds, abApplyNow, abLocked);
}

bool TP_MAKE_THISCALL(HookUnequipObject, EquipManager, Actor* apActor, const ObjectInstance* apInstance, uint32_t aCount, TESForm* apSlot, uint32_t aStackId,
                      bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, TESForm* apSlotToReplace)
{
    TESForm* pItem = apInstance ? apInstance->object : nullptr;
    if (apActor && pItem)
    {
        const auto pExtension = apActor->GetExtension();
        // Removing an item also unequips it, which must go through.
        if (pExtension->IsRemote() && !ScopedEquipOverride::IsOverriden() && !ScopedInventoryOverride::IsOverriden())
            return false;

        if (pExtension->IsLocal() && !ScopedUnequipOverride::IsOverriden())
        {
            EquipmentChangeEvent evt{};
            evt.ActorId = apActor->formID;
            evt.Count = aCount;
            evt.ItemId = pItem->formID;
            evt.EquipSlotId = apSlot ? apSlot->formID : 0;
            evt.Unequip = true;
            evt.IsAmmo = pItem->formType == FormType::Ammo;

            QueueEquipmentChange(apActor, std::move(evt));
        }
    }

    return TiltedPhoques::ThisCall(RealUnequipObject, apThis, apActor, apInstance, aCount, apSlot, aStackId, abQueueEquip, abForceEquip, abPlaySounds, abApplyNow,
                                   apSlotToReplace);
}

static TiltedPhoques::Initializer s_equipmentHooks(
    []()
    {
        static VersionDbPtr<TEquipObject> equipObject(2231392);
        static VersionDbPtr<TUnequipObject> unequipObject(2231395);

        RealEquipObject = equipObject.Get();
        RealUnequipObject = unequipObject.Get();

        TP_HOOK(&RealEquipObject, HookEquipObject);
        TP_HOOK(&RealUnequipObject, HookUnequipObject);
    });
