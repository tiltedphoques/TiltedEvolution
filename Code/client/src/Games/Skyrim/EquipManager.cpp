#ifdef TP_SKYRIM

#include <Games/Skyrim/EquipManager.h>

void EquipManager::Equip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, int aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4)
{

}

void EquipManager::Unequip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, int aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4)
{
    POINTER_SKYRIMSE(EquipManager*, s_singleton, 0x142EC4838 - 0x140000000);

    TP_THIS_FUNCTION(TUnEquip, void, EquipManager, Actor * apActor, TESForm * apItem, BSExtraDataList * apExtraDataList, int aCount, int aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4);

    POINTER_SKYRIMSE(TUnEquip, s_realFunc, 0x140638190 - 0x140000000);

    ThisCall(s_realFunc, *s_singleton.Get(), apActor, apItem, apExtraDataList, aCount, aSlot, aUnk1, aPreventEquip, aUnk2, aUnk3, aUnk4);
}

#endif
