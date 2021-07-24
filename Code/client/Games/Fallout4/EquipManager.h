#pragma once

struct TESForm;
struct BSExtraDataList;
struct Actor;

struct EquipManager
{
    static EquipManager* Get() noexcept;

    void* Equip(Actor* apActor, TESForm* apItem, uint32_t aUnk1, uint32_t aCount, void* aSlot, bool aUnk2, bool aPreventEquip, bool aUnk3, bool aUnk4);
    void* UnEquip(Actor* apActor, TESForm* apItem, int aCount, void* aSlot, int aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, bool aUnk4, void* aUnk5);
};
