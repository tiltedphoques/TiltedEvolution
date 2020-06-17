#pragma once

#ifdef TP_SKYRIM

struct TESForm;
struct BSExtraDataList;
struct Actor;

struct EquipManager
{
    static EquipManager* Get() noexcept;

    void* Equip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, uint64_t aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3);
    void* UnEquip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, uint64_t aSlot, int aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4);
};

#endif
