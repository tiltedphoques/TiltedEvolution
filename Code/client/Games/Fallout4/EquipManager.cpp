#include <TiltedOnlinePCH.h>

#include <EquipManager.h>
#include <Games/References.h>
#include <Games/Overrides.h>
#include <Forms/BGSObjectInstance.h>

#include <Events/EquipmentChangeEvent.h>

#include <World.h>

struct BGSEquipSlot;

struct ObjectEquipParams
{
    uint32_t uiStackID;
    uint32_t uiNumber;
    const BGSEquipSlot* pEquipSlot;
    const BGSEquipSlot* pSlotBeingReplaced;
    bool bQueueEquip;
    bool bForceEquip;
    bool bPlayEquipSounds;
    bool bApplyNow;
    bool bLocked;
    bool bExtraWasDeleted;
};

TP_THIS_FUNCTION(TEquip, void*, EquipManager, Actor* apActor, BGSObjectInstance* apItem, ObjectEquipParams& arParams);
TP_THIS_FUNCTION(TUnEquip, void*, EquipManager, Actor* apActor, BGSObjectInstance* apItem, ObjectEquipParams& arParams);

TEquip* RealEquip = nullptr;
TUnEquip* RealUnEquip = nullptr;

EquipManager* EquipManager::Get() noexcept
{
    POINTER_FALLOUT4(EquipManager*, s_singleton, 1174341);

    return *s_singleton.Get();
}


bool EquipManager::EquipObject(Actor* apActor, BGSObjectInstance& arObject, uint32_t auiStackID, uint32_t auiNumber, const BGSEquipSlot* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, bool abLocked)
{
    spdlog::warn("EquipObject {:X}", arObject.pObject->formID);

    TP_THIS_FUNCTION(TEquipObject, bool, EquipManager, Actor* apActor, BGSObjectInstance& arObject, uint32_t auiStackID, uint32_t auiNumber, const BGSEquipSlot* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, bool abLocked);
    POINTER_FALLOUT4(TEquipObject, equipObject, 988030);

    ScopedEquipOverride _;

    return ThisCall(equipObject, this, apActor, arObject, auiStackID, auiNumber, apSlot, abQueueEquip, abForceEquip, abPlaySounds, abApplyNow, abLocked);
}

bool EquipManager::UnequipObject(Actor* apActor, BGSObjectInstance& arObject, uint32_t auiNumber, const BGSEquipSlot* apSlot, uint32_t auiStackID, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, const BGSEquipSlot* apSlotBeingReplaced)
{
    // TODO: the real unequip happens in some kind of loop over slots.
    // Maybe in fallout 4, multiple items can be on many slots?
    // If so, just call the papyrus function instead, Actor::UnequipItem().
    // Maybe the equipped slot index should be sent even? (see BGSInventoryItem enum)

    TP_THIS_FUNCTION(TUnequipObject, bool, EquipManager, Actor* apActor, BGSObjectInstance& arObject, uint32_t auiNumber, const BGSEquipSlot* apSlot, uint32_t auiStackID, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, const BGSEquipSlot* apSlotBeingReplaced);
    POINTER_FALLOUT4(TUnequipObject, unequipObject, 1292494);

    ScopedEquipOverride _;

    return ThisCall(unequipObject, this, apActor, arObject, auiNumber, apSlot, auiStackID, abQueueEquip, abForceEquip, abPlaySounds, abApplyNow, apSlotBeingReplaced);
}

void* TP_MAKE_THISCALL(EquipHook, EquipManager, Actor* apActor, BGSObjectInstance* apItem, ObjectEquipParams& arParams)
{
    if (!apActor)
        return nullptr;

    const auto pExtension = apActor->GetExtension();
    if (pExtension->IsRemote() && !ScopedEquipOverride::IsOverriden())
    {
        spdlog::info("Actor[{:X}]::Equip(), item form id: {:X}", apActor->formID, apItem->pObject->formID);
        return nullptr;
    }

    if (pExtension->IsLocal())
    {
        EquipmentChangeEvent evt{};
        evt.ActorId = apActor->formID;
        evt.Count = arParams.uiNumber;
        evt.ItemId = apItem->pObject->formID;
        // TODO: equip slot stuff
        //evt.EquipSlotId

        World::Get().GetRunner().Trigger(evt);
    }

    ScopedUnequipOverride _;

    return ThisCall(RealEquip, apThis, apActor, apItem, arParams);
}

void* TP_MAKE_THISCALL(UnEquipHook, EquipManager, Actor* apActor, BGSObjectInstance* apItem, ObjectEquipParams& arParams)
{
    if (!apActor)
        return nullptr;

    const auto pExtension = apActor->GetExtension();
    if (pExtension->IsRemote() && !ScopedEquipOverride::IsOverriden())
        return nullptr;

    if (pExtension->IsLocal())
    {
        EquipmentChangeEvent evt{};
        evt.ActorId = apActor->formID;
        evt.Count = arParams.uiNumber;
        evt.ItemId = apItem->pObject->formID;
        evt.Unequip = true;
        // TODO: equip slot stuff
        //evt.EquipSlotId

        World::Get().GetRunner().Trigger(evt);
    }

    return ThisCall(RealUnEquip, apThis, apActor, apItem, arParams);
}

static TiltedPhoques::Initializer s_equipmentHooks([]()
{
    POINTER_FALLOUT4(TEquip, s_equipFunc, 1474879);
    POINTER_FALLOUT4(TUnEquip, s_unequipFunc, 1265293);

    RealUnEquip = s_unequipFunc.Get();
    RealEquip = s_equipFunc.Get();

    TP_HOOK(&RealUnEquip, UnEquipHook);
    TP_HOOK(&RealEquip, EquipHook);
});
