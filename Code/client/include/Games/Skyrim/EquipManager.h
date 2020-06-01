#pragma once

#ifdef TP_SKYRIM

struct TESForm;
struct BSExtraDataList;
struct Actor;

struct EquipManager
{
    static void Equip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, int aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4);
    static void Unequip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, int aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4);
};

#endif
