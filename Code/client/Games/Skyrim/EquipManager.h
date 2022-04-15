#pragma once

struct TESForm;
struct ExtraDataList;
struct Actor;

struct EquipManager
{
    static EquipManager* Get() noexcept;

    void* Equip(Actor* apActor, TESForm* apItem, ExtraDataList* apExtraDataList, int aCount, TESForm* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySound, bool abApplyNow);
    void* UnEquip(Actor* apActor, TESForm* apItem, ExtraDataList* apExtraDataList, int aCount, TESForm* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySound, bool abApplyNow, TESForm* apSlotToReplace);
    void* EquipSpell(Actor* apActor, TESForm* apSpell, uint32_t aSlotId);
    void* UnEquipSpell(Actor* apActor, TESForm* apSpell, uint32_t aSlotId);
    void* EquipShout(Actor* apActor, TESForm* apShout);
    void* UnEquipShout(Actor* apActor, TESForm* apShout);
};
