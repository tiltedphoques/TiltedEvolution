#pragma once

#ifdef TP_SKYRIM

struct TESForm;
struct BSExtraDataList;
struct Actor;

struct EquipManager
{
    static EquipManager* Get() noexcept;

    void* EquipSpell(Actor* apActor, TESForm* apItem, uint32_t aSlotId);
    void* UnEquipSpell(Actor* apActor, TESForm* apItem, uint32_t aSlotId);
    void* Equip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, void* aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3);
    void* UnEquip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, void* aSlot, int aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4);
};

#endif
