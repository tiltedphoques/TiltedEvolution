#include <stdafx.h>

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
TP_THIS_FUNCTION(TEquipSpell, void*, EquipManager, Actor* apActor, TESForm* apSpell, void* apSlot);
TP_THIS_FUNCTION(TUnEquipSpell, void*, EquipManager, Actor* apActor, TESForm* apSpell, void* apSlot);
TP_THIS_FUNCTION(TEquipShout, void*, EquipManager, Actor* apActor, TESForm* apShout, void* apReturn);
TP_THIS_FUNCTION(TUnEquipShout, void*, EquipManager, Actor* apActor, TESForm* apShout, void* apReturn);

TUnEquip* RealUnEquip = nullptr;
TEquip* RealEquip = nullptr;
TEquipSpell* RealEquipSpell = nullptr;
TUnEquipSpell* RealUnEquipSpell = nullptr;
TEquipShout* RealEquipShout = nullptr;
TUnEquipShout* RealUnEquipShout = nullptr;

EquipManager* EquipManager::Get() noexcept
{
    POINTER_SKYRIMSE(EquipManager*, s_singleton, 0x142EC4838 - 0x140000000);

    return *s_singleton.Get();
}

void* EquipManager::EquipSpell(Actor* apActor, TESForm* apSpell, uint32_t aSlotId)
{
    TP_THIS_FUNCTION(TEquipSpellInternal, void*, EquipManager, Actor*, TESForm*, uint32_t);
    POINTER_SKYRIMSE(TEquipSpellInternal, s_equipFunc, 0x140637C30 - 0x140000000);

    ScopedEquipOverride equipOverride;

    const auto result = ThisCall(s_equipFunc, this, apActor, apSpell, aSlotId);

    return result;
}

void* EquipManager::UnEquipSpell(Actor* apActor, TESForm* apSpell, uint32_t aSlotId)
{
    TP_THIS_FUNCTION(TUnEquipSpellInternal, void*, EquipManager, Actor*, TESForm*, uint32_t);
    POINTER_SKYRIMSE(TUnEquipSpellInternal, s_unequipFunc, 0x1406383B0 - 0x140000000);

    ScopedEquipOverride equipOverride;

    const auto result = ThisCall(s_unequipFunc, this, apActor, apSpell, aSlotId);

    return result;
}

void* EquipManager::EquipShout(Actor* apActor, TESForm* apShout)
{
    uint8_t data[0x100];

    ScopedEquipOverride equipOverride;

    const auto result = ThisCall(RealEquipShout, this, apActor, apShout, data);

    return result;
}

void* EquipManager::UnEquipShout(Actor* apActor, TESForm* apShout)
{
    uint8_t data[0x100];

    ScopedEquipOverride equipOverride;

    const auto result = ThisCall(RealUnEquipShout, this, apActor, apShout, data);

    return result;
}


void* EquipManager::Equip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, void* aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3)
{
    TP_THIS_FUNCTION(TEquipInternal, void*, EquipManager, Actor * apActor, TESForm * apItem, BSExtraDataList * apExtraDataList, int aCount, void* aSlot, bool aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3);
    POINTER_SKYRIMSE(TEquipInternal, s_equipFunc, 0x140637A80 - 0x140000000);

    ScopedEquipOverride equipOverride;

    const auto result = ThisCall(s_equipFunc, this, apActor, apItem, apExtraDataList, aCount, aSlot, aUnk1, aPreventEquip, aUnk2, aUnk3);

    return result;
}

void* EquipManager::UnEquip(Actor* apActor, TESForm* apItem, BSExtraDataList* apExtraDataList, int aCount, void* aSlot, int aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4)
{
    TP_THIS_FUNCTION(TUnEquipInternal, void*, EquipManager, Actor * apActor, TESForm * apItem, BSExtraDataList * apExtraDataList, int aCount, void* aSlot, int aUnk1, bool aPreventEquip, bool aUnk2, bool aUnk3, void* aUnk4);
    POINTER_SKYRIMSE(TUnEquipInternal, s_unequipFunc, 0x140638190 - 0x140000000);

    ScopedEquipOverride equipOverride;

    const auto result = ThisCall(s_unequipFunc, this, apActor, apItem, apExtraDataList, aCount, aSlot, aUnk1, aPreventEquip, aUnk2, aUnk3, aUnk4);

    return result;
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

void* TP_MAKE_THISCALL(EquipSpellHook, EquipManager, Actor* apActor, TESForm* apSpell, void* apSlot)
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

    return ThisCall(RealEquipSpell, apThis, apActor, apSpell, apSlot);
}

void* TP_MAKE_THISCALL(UnEquipSpellHook, EquipManager, Actor* apActor, TESForm* apSpell, void* apSlot)
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

    return ThisCall(RealUnEquipSpell, apThis, apActor, apSpell, apSlot);
}

void* TP_MAKE_THISCALL(EquipShoutHook, EquipManager, Actor* apActor, TESForm* apShout, void* apReturn)
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

    return ThisCall(RealEquipShout, apThis, apActor, apShout, apReturn);
}

void* TP_MAKE_THISCALL(UnEquipShoutHook, EquipManager, Actor* apActor, TESForm* apShout, void* apReturn)
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

    return ThisCall(RealUnEquipShout, apThis, apActor, apShout, apReturn);
}


static TiltedPhoques::Initializer s_equipmentHooks([]()
    {
        POINTER_SKYRIMSE(TEquip, s_equipFunc, 0x140639E20 - 0x140000000);
        POINTER_SKYRIMSE(TUnEquip, s_unequipFunc, 0x14063A6B0 - 0x140000000);
        POINTER_SKYRIMSE(TEquipSpell, s_equipSpellFunc, 0x140639B70 - 0x140000000);
        POINTER_SKYRIMSE(TUnEquipSpell, s_unequipSpellFunc, 0x140638340 - 0x140000000);
        POINTER_SKYRIMSE(TEquipShout, s_equipShoutFunc, 0x14063A0D0 - 0x140000000);
        POINTER_SKYRIMSE(TUnEquipShout, s_unequipShoutFunc, 0x14063A900 - 0x140000000);

        RealUnEquip = s_unequipFunc.Get();
        RealEquip = s_equipFunc.Get();
        RealEquipSpell = s_equipSpellFunc.Get();
        RealUnEquipSpell = s_unequipSpellFunc.Get();
        RealEquipShout = s_equipShoutFunc.Get();
        RealUnEquipShout = s_unequipShoutFunc.Get();

        TP_HOOK(&RealUnEquip, UnEquipHook);
        TP_HOOK(&RealEquip, EquipHook);
        TP_HOOK(&RealEquipSpell, EquipSpellHook);
        TP_HOOK(&RealUnEquipSpell, UnEquipSpellHook);
        TP_HOOK(&RealEquipShout, EquipShoutHook);
        TP_HOOK(&RealUnEquipShout, UnEquipShoutHook);
    });
