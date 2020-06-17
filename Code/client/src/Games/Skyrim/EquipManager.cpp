#ifdef TP_SKYRIM

#include <Games/Skyrim/EquipManager.h>
#include <Games/References.h>

struct EquipData
{
    BSExtraDataList* extraDataList; // 0
    int count; // 8
    int padC; // C
    uint64_t slot; // 10
    void* unk18; // 18
    bool preventEquip;
    bool unkb1;
    bool unkb2;
    bool unkb3;
    bool unkb4;
};

struct UnEquipData
{
    BSExtraDataList* extraDataList; // 0
    int count; // 8
    int padC; // C
    uint64_t slot; // 10
    void* unk18; // 18
    bool unkb0;
    bool preventEquip;
    bool unkb2;
    bool unkb3;
    bool unkb4;
};

TP_THIS_FUNCTION(TUnEquip, void*, EquipManager, Actor* apActor, TESForm* apItem, UnEquipData* apData);
TP_THIS_FUNCTION(TEquip, void*, EquipManager, Actor* apActor, TESForm* apItem, EquipData* apData);

TUnEquip* RealUnEquip = nullptr;
TEquip* RealEquip = nullptr;

static thread_local bool s_callPass = false;

EquipManager* EquipManager::Get() noexcept
{
    POINTER_SKYRIMSE(EquipManager*, s_singleton, 0x142EC4838 - 0x140000000);

    return *s_singleton.Get();
}

void* EquipManager::Equip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, uint64_t aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3)
{
    TP_THIS_FUNCTION(TEquipInternal, void*, EquipManager, Actor * apActor, TESForm * apItem, BSExtraDataList * apExtraDataList, int aCount, uint64_t aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3);
    POINTER_SKYRIMSE(TEquipInternal, s_equipFunc, 0x140637A80 - 0x140000000);

    s_callPass = true;
    const auto result = ThisCall(s_equipFunc, this, apActor, apItem, apExtraDataList, aCount, aSlot, aUnk1, aPreventEquip, aUnk2, aUnk3);
    s_callPass = false;

    return result;
}

void* EquipManager::UnEquip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, uint64_t aSlot, int aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4)
{
    TP_THIS_FUNCTION(TUnEquipInternal, void*, EquipManager, Actor * apActor, TESForm * apItem, BSExtraDataList * apExtraDataList, int aCount, uint64_t aSlot, int aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4);
    POINTER_SKYRIMSE(TUnEquipInternal, s_unequipFunc, 0x140638190 - 0x140000000);

    s_callPass = true;
    const auto result = ThisCall(s_unequipFunc, this, apActor, apItem, apExtraDataList, aCount, aSlot, aUnk1, aPreventEquip, aUnk2, aUnk3, aUnk4);
    s_callPass = false;

    return result;
}

void* TP_MAKE_THISCALL(UnEquipHook, EquipManager, Actor* apActor, TESForm* apItem, UnEquipData* apData)
{
    if (!s_callPass && apActor->GetExtension() && apActor->GetExtension()->IsRemote())
        return nullptr;

    return ThisCall(RealUnEquip, apThis, apActor, apItem, apData);
}

void* TP_MAKE_THISCALL(EquipHook, EquipManager, Actor* apActor, TESForm* apItem, EquipData* apData)
{
    if (!s_callPass && apActor->GetExtension() && apActor->GetExtension()->IsRemote())
        return nullptr;

    return ThisCall(RealEquip, apThis, apActor, apItem, apData);
}

static TiltedPhoques::Initializer s_equipmentHooks([]()
    {
        POINTER_SKYRIMSE(TEquip, s_equipFunc, 0x140639E20 - 0x140000000);
        POINTER_SKYRIMSE(TUnEquip, s_unequipFunc, 0x14063A6B0 - 0x140000000);

        RealUnEquip = s_unequipFunc.Get();
        RealEquip = s_equipFunc.Get();

        TP_HOOK(&RealUnEquip, UnEquipHook);
        TP_HOOK(&RealEquip, EquipHook);
    });

#endif
