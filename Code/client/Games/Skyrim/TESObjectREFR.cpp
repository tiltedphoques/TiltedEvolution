#include <TiltedOnlinePCH.h>

#include <Games/References.h>
#include <Games/Overrides.h>

#include <World.h>
#include <Services/PapyrusService.h>
#include <Events/ActivateEvent.h>

TP_THIS_FUNCTION(TActivate, void, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing);
TP_THIS_FUNCTION(TAddInventoryItem, void, TESObjectREFR, TESBoundObject* apItem, BSExtraDataList* apExtraData, uint32_t aCount, TESObjectREFR* apOldOwner);
TP_THIS_FUNCTION(TRemoveInventoryItem, void*, TESObjectREFR, TESBoundObject* apItem, uint32_t aCount, uint32_t aUnk1, BSExtraDataList* apExtraData, TESObjectREFR* apNewOwner, NiPoint3* apUnk2, NiPoint3* apUnk3);

static TActivate* RealActivate = nullptr;
static TAddInventoryItem* RealAddInventoryItem = nullptr;
static TRemoveInventoryItem* RealRemoveInventoryItem = nullptr;

#ifdef SAVE_STUFF

#include <Games/Skyrim/SaveLoad.h>

void TESObjectREFR::Save_Reversed(const uint32_t aChangeFlags, Buffer::Writer& aWriter)
{
    TESForm::Save_Reversed(aChangeFlags, aWriter);

    if(aChangeFlags & CHANGE_REFR_BASEOBJECT)
    {
        // save baseForm->formID;
        // we don't because each player has it's own form id system
    }

    if(aChangeFlags & CHANGE_REFR_SCALE)
    {
        // So skyrim does some weird conversion shit here, we are going to do the same for now
        float fScale = scale;
        fScale /= 100.f;
        aWriter.WriteBytes((uint8_t*)& fScale, 4);
    }

    // So skyrim 
    uint32_t extraFlags = 0xA6021C40;
    if(formType == Character)
        extraFlags = 0xA6061840;

    if(aChangeFlags & extraFlags)
    {
        // We have flags to save
    }

    if(aChangeFlags & (CHANGE_REFR_INVENTORY | CHANGE_REFR_LEVELED_INVENTORY))
    {
        
    }

    if (aChangeFlags & CHANGE_REFR_ANIMATION)
    {
		// do something with animations
		// get extradata 0x41
    }


}

#endif

ExtraContainerChanges::Data* TESObjectREFR::GetContainerChanges() const noexcept
{
    TP_THIS_FUNCTION(TGetContainterChanges, ExtraContainerChanges::Data*, const TESObjectREFR);

    POINTER_SKYRIMSE(TGetContainterChanges, s_getContainerChangs, 0x1401D8E40 - 0x140000000);
    
    return ThisCall(s_getContainerChangs, this);
}

void TESObjectREFR::SaveInventory(BGSSaveFormBuffer* apBuffer) const noexcept
{
    GetContainerChanges()->Save(apBuffer);
}

void TESObjectREFR::LoadInventory(BGSLoadFormBuffer* apBuffer) noexcept
{
    GetContainerChanges()->Load(apBuffer);
}

void TESObjectREFR::RemoveAllItems() noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(void, ObjectReference, RemoveAllItems, TESObjectREFR*, bool, bool);

    ScopedEquipOverride equipOverride;

    s_pRemoveAllItems(this, nullptr, false, true);
}

void TESObjectREFR::Activate(TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* aObjectToGet, int32_t aCount, char aDefaultProcessing) noexcept
{
    return ThisCall(RealActivate, this, apActivator, aUnk1, aObjectToGet, aCount, aDefaultProcessing);
}

void TP_MAKE_THISCALL(HookActivate, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing)
{
    auto* pActivator = RTTI_CAST(apActivator, TESObjectREFR, Actor);
    if (pActivator)
        World::Get().GetRunner().Trigger(ActivateEvent(apThis, pActivator, apObjectToGet, aUnk1, aCount, aDefaultProcessing));

    return ThisCall(RealActivate, apThis, apActivator, aUnk1, apObjectToGet, aCount, aDefaultProcessing);
}

void TP_MAKE_THISCALL(HookAddInventoryItem, TESObjectREFR, TESBoundObject* apItem, BSExtraDataList* apExtraData, uint32_t aCount, TESObjectREFR* apOldOwner)
{
    spdlog::warn("Add inventory item");
    ThisCall(RealAddInventoryItem, apThis, apItem, apExtraData, aCount, apOldOwner);
}

void* TP_MAKE_THISCALL(HookRemoveInventoryItem, TESObjectREFR, TESBoundObject* apItem, uint32_t aCount, uint32_t aUnk1, BSExtraDataList* apExtraData, TESObjectREFR* apNewOwner, NiPoint3* apUnk2, NiPoint3* apUnk3)
{
    spdlog::critical("Remove inventory item");
    return ThisCall(RealRemoveInventoryItem, apThis, apItem, aCount, aUnk1, apExtraData, apNewOwner, apUnk2, apUnk3);
}

static TiltedPhoques::Initializer s_objectReferencesHooks([]() {
        POINTER_SKYRIMSE(TActivate, s_activate, 0x140296C00 - 0x140000000);
        POINTER_SKYRIMSE(TAddInventoryItem, s_addInventoryItem, 0x14028E680 - 0x140000000);
        POINTER_SKYRIMSE(TRemoveInventoryItem, s_removeInventoryItem, 0x14028D9E0 - 0x140000000);

        RealActivate = s_activate.Get();
        RealAddInventoryItem = s_addInventoryItem.Get();
        RealRemoveInventoryItem = s_removeInventoryItem.Get();

        TP_HOOK(&RealActivate, HookActivate);
        TP_HOOK(&RealAddInventoryItem, HookAddInventoryItem);
        TP_HOOK(&RealRemoveInventoryItem, HookRemoveInventoryItem);
});
