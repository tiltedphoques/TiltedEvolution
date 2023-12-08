#include "ActorEquipManager.h"

ActorEquipManager* ActorEquipManager::Get() noexcept
{
    return (ActorEquipManager*)0x1458F7378; // 879425
}

// In tilted evo, we hooked a few functions lower than this and called the below funcs.
// Not sure if that is necessary anymore, try hooking these first.

// TODO: hook this
bool ActorEquipManager::EquipObject(Actor* apActor, BGSObjectInstance& arObject, const BGSEquipSlot* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, bool abLocked)
{
    using TEquipObject = bool(ActorEquipManager*, Actor* apActor, BGSObjectInstance& arObject, const BGSEquipSlot* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, bool abLocked);
    TEquipObject* equipObject = (TEquipObject*)0x1424EA41C; // 151991
    return equipObject(this, apActor, arObject, apSlot, abQueueEquip, abForceEquip, abPlaySounds, abApplyNow, abLocked);
}

// TODO: hook this
bool ActorEquipManager::UnequipObject(
    Actor* apActor, BGSObjectInstance& arObject, const BGSEquipSlot* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow,
    const BGSEquipSlot* apSlotBeingReplaced)
{
    using TUnequipObject = bool(
        ActorEquipManager*, Actor * apActor, BGSObjectInstance & arObject, const BGSEquipSlot* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow,
        const BGSEquipSlot* apSlotBeingReplaced);
    TUnequipObject* unequipObject = (TUnequipObject*)0x1424EB0A4; // 152007
    return unequipObject(this, apActor, arObject, apSlot, abQueueEquip, abForceEquip, abPlaySounds, abApplyNow, apSlotBeingReplaced);
}
