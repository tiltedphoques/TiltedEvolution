#pragma once

struct Actor;
struct BGSObjectInstance;
struct BGSEquipSlot;

struct ActorEquipManager
{
    static ActorEquipManager* Get() noexcept;

    bool
    EquipObject(Actor* apActor, BGSObjectInstance& arObject, const BGSEquipSlot* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, bool abLocked);
    bool UnequipObject(
        Actor* apActor, BGSObjectInstance& arObject, const BGSEquipSlot* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySounds,
        bool abApplyNow, const BGSEquipSlot* apSlotBeingReplaced);
};
