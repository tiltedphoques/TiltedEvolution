#include <TiltedOnlinePCH.h>

#include <EquipManager.h>
#include <Games/References.h>
#include <Games/Overrides.h>

#include <Events/EquipmentChangeEvent.h>

#include <World.h>

struct EquipData
{
    BSExtraDataList* extraDataList; // 0
    int count; // 8
    int padC; // C
    struct BGSEquipSlot* slot; // 10
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
    struct BGSEquipSlot* slot; // 10
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

EquipManager* EquipManager::Get() noexcept
{
    POINTER_FALLOUT4(EquipManager*, s_singleton, 0x1459D75C8 - 0x140000000);

    return *s_singleton.Get();
}

void* EquipManager::Equip(Actor* apActor, TESForm* apItem, uint32_t aUnk1, uint32_t aCount, void* aSlot, bool aUnk2, bool aPreventEquip, bool aUnk3, bool aUnk4)
{
    TP_THIS_FUNCTION(TEquipInternal, void*, EquipManager, Actor* apActor, TESForm* apItem, uint32_t aUnk1, uint32_t aCount, void* aSlot, bool aUnk2, bool aPreventEquip, bool aUnk3, bool aUnk4);
    POINTER_FALLOUT4(TEquipInternal, s_equipFunc, 0x140E1BCC0 - 0x140000000);

    ScopedEquipOverride equipOverride;

    const auto result = ThisCall(s_equipFunc, this, apActor, apItem, aUnk1, aCount, aSlot, aUnk2, aPreventEquip, aUnk3, aUnk4);

    return result;
}

void* EquipManager::UnEquip(Actor* apActor, TESForm* apItem, int aCount, void* aSlot, int aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, bool aUnk4, void* aUnk5)
{
    TP_THIS_FUNCTION(TUnEquipInternal, void*, EquipManager, Actor* apActor, TESForm* apItem, int aCount, void* aSlot, int aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, bool aUnk4, void* aUnk5);
    POINTER_FALLOUT4(TUnEquipInternal, s_unequipFunc, 0x140E1C0A0 - 0x140000000);

    ScopedEquipOverride equipOverride;

    const auto result = ThisCall(s_unequipFunc, this, apActor, apItem, aCount, aSlot, aUnk1, aPreventEquip, aUnk2, aUnk3, aUnk4, aUnk5);

    return result;
}

void* TP_MAKE_THISCALL(EquipHook, EquipManager, Actor* apActor, TESForm* apItem, EquipData* apData)
{
    if (!apActor)
        return nullptr;

    const auto pExtension = apActor->GetExtension();
    if (pExtension->IsRemote() && !ScopedEquipOverride::IsOverriden())
    {
        spdlog::info("Actor[{:X}::Equip()", apActor->formID);
        return nullptr;
    }

    if (pExtension->IsLocal())
    {
        EquipmentChangeEvent evt;
        evt.ActorId = apActor->formID;

        World::Get().GetRunner().Trigger(evt);
    }

    return ThisCall(RealEquip, apThis, apActor, apItem, apData);
}

void* TP_MAKE_THISCALL(UnEquipHook, EquipManager, Actor* apActor, TESForm* apItem, UnEquipData* apData)
{
    if (!apActor)
        return nullptr;

    const auto pExtension = apActor->GetExtension();
    if (pExtension->IsRemote() && !ScopedEquipOverride::IsOverriden())
        return nullptr;

    if (pExtension->IsLocal())
    {
        EquipmentChangeEvent evt;
        evt.ActorId = apActor->formID;

        World::Get().GetRunner().Trigger(evt);
    }

    return ThisCall(RealUnEquip, apThis, apActor, apItem, apData);
}

static TiltedPhoques::Initializer s_equipmentHooks([]()
    {
        POINTER_FALLOUT4(TEquip, s_equipFunc, 0x140E1EB40 - 0x140000000);
        POINTER_FALLOUT4(TUnEquip, s_unequipFunc, 0x140E20390 - 0x140000000);

        RealUnEquip = s_unequipFunc.Get();
        RealEquip = s_equipFunc.Get();

        TP_HOOK(&RealUnEquip, UnEquipHook);
        TP_HOOK(&RealEquip, EquipHook);
    });
