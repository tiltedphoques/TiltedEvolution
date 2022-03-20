#pragma once

struct TESForm;
struct ExtraDataList;
struct Actor;
struct BGSObjectInstance;
struct BGSEquipSlot;

struct EquipManager
{
    static EquipManager* Get() noexcept;

    bool EquipObject(Actor* apActor, BGSObjectInstance& arObject, uint32_t auiStackID, uint32_t auiNumber, const BGSEquipSlot* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, bool abLocked);
    bool UnequipObject(Actor* apActor, BGSObjectInstance& arObject, uint32_t auiNumber, const BGSEquipSlot* apSlot, uint32_t auiStackID, bool abQueueEquip, bool abForceEquip, bool abPlaySounds, bool abApplyNow, const BGSEquipSlot* apSlotBeingReplaced);
};
