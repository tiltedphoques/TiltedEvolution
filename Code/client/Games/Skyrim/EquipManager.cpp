#include <TiltedOnlinePCH.h>

#include <EquipManager.h>
#include <Games/References.h>
#include <Games/Overrides.h>

#include <Events/EquipmentChangeEvent.h>

#include <World.h>

#include <Games/Skyrim/Forms/TESAmmo.h>
#include <AI/AIProcess.h>
#include <Games/Skyrim/Misc/MiddleProcess.h>
#include <DefaultObjectManager.h>

struct BGSEquipSlot : TESForm
{
};

struct EquipData
{
    ExtraDataList* pExtraDataList; // 0
    int32_t count; // 8
    BGSEquipSlot* pSlot; // 10
    BGSEquipSlot* pSlotToReplace; // 18
    bool bQueueEquip;
    bool bForceEquip;
    bool bPlaySound;
    bool bApplyNow;
    bool bUnk1;
};
static_assert(sizeof(EquipData) == 0x28);

struct MagicEquipData
{
    BGSEquipSlot* pEquipSlot;
    bool bQueueEquip;
    bool bForceEquip;
};
static_assert(sizeof(MagicEquipData) == 0x10);

struct ShoutEquipData
{
    void* pUnk1;
    bool bUnk2;
};
static_assert(sizeof(MagicEquipData) == 0x10);

TP_THIS_FUNCTION(TEquip, void*, EquipManager, Actor* apActor, TESForm* apItem, EquipData* apData);
TP_THIS_FUNCTION(TUnEquip, void*, EquipManager, Actor* apActor, TESForm* apItem, EquipData* apData);
TP_THIS_FUNCTION(TEquipSpell, void*, EquipManager, Actor* apActor, TESForm* apSpell, MagicEquipData* apData);
TP_THIS_FUNCTION(TUnEquipSpell, void*, EquipManager, Actor* apActor, TESForm* apSpell, MagicEquipData* apData);
TP_THIS_FUNCTION(TEquipShout, void*, EquipManager, Actor* apActor, TESForm* apShout, ShoutEquipData* apData);
TP_THIS_FUNCTION(TUnEquipShout, void*, EquipManager, Actor* apActor, TESForm* apShout, ShoutEquipData* apData);

TUnEquip* RealUnEquip = nullptr;
TEquip* RealEquip = nullptr;
TEquipSpell* RealEquipSpell = nullptr;
TUnEquipSpell* RealUnEquipSpell = nullptr;
TEquipShout* RealEquipShout = nullptr;
TUnEquipShout* RealUnEquipShout = nullptr;

EquipManager* EquipManager::Get() noexcept
{
    POINTER_SKYRIMSE(EquipManager*, s_singleton, 400636);

    return *s_singleton.Get();
}

void* EquipManager::Equip(Actor* apActor, TESForm* apItem, ExtraDataList* apExtraDataList, int aCount, TESForm* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySound, bool abApplyNow)
{
    TP_THIS_FUNCTION(TEquipInternal, void*, EquipManager, Actor * apActor, TESForm * apItem, ExtraDataList * apExtraDataList, int aCount, TESForm* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySound, bool abApplyNow);
    POINTER_SKYRIMSE(TEquipInternal, s_equipFunc, 38894);

    ScopedEquipOverride equipOverride;

    spdlog::debug("Call Actor[{:X}]::Equip(), item id: {:X}, extra data? {}, count: {}", apActor->formID, apItem->formID, (bool)apExtraDataList, aCount);

    return ThisCall(s_equipFunc, this, apActor, apItem, apExtraDataList, aCount, apSlot, abQueueEquip, abForceEquip, abPlaySound, abApplyNow);
}

void* EquipManager::UnEquip(Actor* apActor, TESForm* apItem, ExtraDataList* apExtraDataList, int aCount, TESForm* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySound, bool abApplyNow, TESForm* apSlotToReplace)
{
    TP_THIS_FUNCTION(TUnEquipInternal, void*, EquipManager, Actor * apActor, TESForm * apItem, ExtraDataList * apExtraDataList, int aCount, TESForm* apSlot, bool abQueueEquip, bool abForceEquip, bool abPlaySound, bool abApplyNow, TESForm* apSlotToReplace);
    POINTER_SKYRIMSE(TUnEquipInternal, s_unequipFunc, 38901);

    ScopedEquipOverride equipOverride;

    spdlog::debug("Call Actor[{:X}]::UnEquip(), item id: {:X}, extra data? {}, count: {}", apActor->formID, apItem->formID, (bool)apExtraDataList, aCount);

    return ThisCall(s_unequipFunc, this, apActor, apItem, apExtraDataList, aCount, apSlot, abQueueEquip, abForceEquip, abPlaySound, abApplyNow, apSlotToReplace);
}

void* EquipManager::EquipSpell(Actor* apActor, TESForm* apSpell, uint32_t aSlotId)
{
    TP_THIS_FUNCTION(TEquipSpellInternal, void*, EquipManager, Actor*, TESForm*, uint32_t);
    POINTER_SKYRIMSE(TEquipSpellInternal, s_equipFunc, 38896);

    ScopedEquipOverride equipOverride;

    return ThisCall(s_equipFunc, this, apActor, apSpell, aSlotId);
}

void* EquipManager::UnEquipSpell(Actor* apActor, TESForm* apSpell, uint32_t aSlotId)
{
    TP_THIS_FUNCTION(TUnEquipSpellInternal, void*, EquipManager, Actor*, TESForm*, uint32_t);
    POINTER_SKYRIMSE(TUnEquipSpellInternal, s_unequipFunc, 38903);

    ScopedEquipOverride equipOverride;

    return ThisCall(s_unequipFunc, this, apActor, apSpell, aSlotId);
}

void* EquipManager::EquipShout(Actor* apActor, TESForm* apShout)
{
    TP_THIS_FUNCTION(TEquipShoutInternal, void*, EquipManager, Actor*, TESForm*);
    POINTER_SKYRIMSE(TEquipShoutInternal, s_equipFunc, 38897);

    ScopedEquipOverride equipOverride;

    return ThisCall(s_equipFunc, this, apActor, apShout);
}

void* EquipManager::UnEquipShout(Actor* apActor, TESForm* apShout)
{
    TP_THIS_FUNCTION(TUnEquipShoutInternal, void*, EquipManager, Actor*, TESForm*);
    POINTER_SKYRIMSE(TUnEquipShoutInternal, s_unequipFunc, 38903);

    ScopedEquipOverride equipOverride;

    return ThisCall(s_unequipFunc, this, apActor, apShout);
}

void* TP_MAKE_THISCALL(EquipHook, EquipManager, Actor* apActor, TESForm* apItem, EquipData* apData)
{
    if (!apActor)
        return nullptr;

    const auto pExtension = apActor->GetExtension();
    if (pExtension->IsRemote())
    {
        spdlog::debug("Actor[{:X}]::Equip(), item form id: {:X}", apActor->formID, apItem->formID);
        if (!ScopedEquipOverride::IsOverriden())
            return nullptr;
    }

    if (pExtension->IsLocal())
    {
        EquipmentChangeEvent evt{};
        evt.ActorId = apActor->formID;
        evt.Count = apData->count;
        evt.ItemId = apItem->formID;
        evt.EquipSlotId = apData->pSlot ? apData->pSlot->formID : 0;
        evt.IsAmmo = apItem->formType == FormType::Ammo;

        World::Get().GetRunner().Trigger(evt);
    }

    ScopedUnequipOverride _;

    return ThisCall(RealEquip, apThis, apActor, apItem, apData);
}

void* TP_MAKE_THISCALL(UnEquipHook, EquipManager, Actor* apActor, TESForm* apItem, EquipData* apData)
{
    if (!apActor)
        return nullptr;

    const auto pExtension = apActor->GetExtension();
    if (pExtension->IsRemote())
    {
        spdlog::debug("Actor[{:X}]::Unequip(), item form id: {:X}, IsOverridden, equip: {}, inventory: {}", apActor->formID, apItem->formID, ScopedEquipOverride::IsOverriden(), ScopedInventoryOverride::IsOverriden());
        // The ScopedInventoryOverride check is here to allow the item to be unequipped if it is removed
        // Without this check, the game will not accept null as a return, and it'll keep trying to unequip infinitely
        if (!ScopedEquipOverride::IsOverriden() && !ScopedInventoryOverride::IsOverriden())
            return nullptr;
    }

    if (pExtension->IsLocal() && !ScopedUnequipOverride::IsOverriden())
    {
        EquipmentChangeEvent evt{};
        evt.ActorId = apActor->formID;
        evt.Count = apData->count;
        evt.ItemId = apItem->formID;
        evt.EquipSlotId = apData->pSlot ? apData->pSlot->formID : 0;
        evt.Unequip = true;
        evt.IsAmmo = apItem->formType == FormType::Ammo;

        World::Get().GetRunner().Trigger(evt);
    }

    spdlog::debug("UnEquipHook, actor: {:X}", apActor->formID);

    return ThisCall(RealUnEquip, apThis, apActor, apItem, apData);
}

void* TP_MAKE_THISCALL(EquipSpellHook, EquipManager, Actor* apActor, TESForm* apSpell, MagicEquipData* apData)
{
    if (!apActor)
        return nullptr;

    const auto pExtension = apActor->GetExtension();
    if (pExtension->IsRemote() && !ScopedEquipOverride::IsOverriden())
        return nullptr;

    if (pExtension->IsLocal())
    {
        EquipmentChangeEvent evt{};
        evt.ActorId = apActor->formID;
        evt.ItemId = apSpell->formID;
        evt.EquipSlotId = apData->pEquipSlot->formID;
        evt.IsSpell = true;

        World::Get().GetRunner().Trigger(evt);
    }

    ScopedUnequipOverride _;

    return ThisCall(RealEquipSpell, apThis, apActor, apSpell, apData);
}

void* TP_MAKE_THISCALL(UnEquipSpellHook, EquipManager, Actor* apActor, TESForm* apSpell, MagicEquipData* apData)
{
    if (!apActor)
        return nullptr;

    const auto pExtension = apActor->GetExtension();
    if (pExtension->IsRemote() && !ScopedEquipOverride::IsOverriden())
        return nullptr;

    if (pExtension->IsLocal() && !ScopedUnequipOverride::IsOverriden())
    {
        EquipmentChangeEvent evt{};
        evt.ActorId = apActor->formID;
        evt.ItemId = apSpell->formID;
        evt.EquipSlotId = apData->pEquipSlot->formID;
        evt.Unequip = true;
        evt.IsSpell = true;

        World::Get().GetRunner().Trigger(evt);
    }

    return ThisCall(RealUnEquipSpell, apThis, apActor, apSpell, apData);
}

void* TP_MAKE_THISCALL(EquipShoutHook, EquipManager, Actor* apActor, TESForm* apShout, ShoutEquipData* apData)
{
    if (!apActor)
        return nullptr;

    const auto pExtension = apActor->GetExtension();
    if (pExtension->IsRemote() && !ScopedEquipOverride::IsOverriden())
        return nullptr;

    if (pExtension->IsLocal())
    {
        EquipmentChangeEvent evt{};
        evt.ActorId = apActor->formID;
        evt.ItemId = apShout->formID;
        evt.IsShout = true;

        World::Get().GetRunner().Trigger(evt);
    }

    ScopedUnequipOverride _;

    return ThisCall(RealEquipShout, apThis, apActor, apShout, apData);
}

void* TP_MAKE_THISCALL(UnEquipShoutHook, EquipManager, Actor* apActor, TESForm* apShout, ShoutEquipData* apData)
{
    if (!apActor)
        return nullptr;

    const auto pExtension = apActor->GetExtension();
    if (pExtension->IsRemote() && !ScopedEquipOverride::IsOverriden())
        return nullptr;

    if (pExtension->IsLocal() && !ScopedUnequipOverride::IsOverriden())
    {
        EquipmentChangeEvent evt{};
        evt.ActorId = apActor->formID;
        evt.ItemId = apShout->formID;
        evt.Unequip = true;
        evt.IsShout = true;

        World::Get().GetRunner().Trigger(evt);
    }

    return ThisCall(RealUnEquipShout, apThis, apActor, apShout, apData);
}


static TiltedPhoques::Initializer s_equipmentHooks([]()
{
    POINTER_SKYRIMSE(TEquip, s_equipFunc, 38929);
    POINTER_SKYRIMSE(TUnEquip, s_unequipFunc, 38934);
    POINTER_SKYRIMSE(TEquipSpell, s_equipSpellFunc, 38928);
    POINTER_SKYRIMSE(TUnEquipSpell, s_unequipSpellFunc, 38933);
    POINTER_SKYRIMSE(TEquipShout, s_equipShoutFunc, 38930);
    POINTER_SKYRIMSE(TUnEquipShout, s_unequipShoutFunc, 38935);

    RealEquip = s_equipFunc.Get();
    RealUnEquip = s_unequipFunc.Get();
    RealEquipSpell = s_equipSpellFunc.Get();
    RealUnEquipSpell = s_unequipSpellFunc.Get();
    RealEquipShout = s_equipShoutFunc.Get();
    RealUnEquipShout = s_unequipShoutFunc.Get();

    TP_HOOK(&RealEquip, EquipHook);
    TP_HOOK(&RealUnEquip, UnEquipHook);
    TP_HOOK(&RealEquipSpell, EquipSpellHook);
    TP_HOOK(&RealUnEquipSpell, UnEquipSpellHook);
    TP_HOOK(&RealEquipShout, EquipShoutHook);
    TP_HOOK(&RealUnEquipShout, UnEquipShoutHook);
});
