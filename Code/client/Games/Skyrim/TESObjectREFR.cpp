#include <TiltedOnlinePCH.h>

#include <Games/References.h>
#include <Games/Overrides.h>

#include <World.h>
#include <Services/PapyrusService.h>
#include <Events/ActivateEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <Events/ScriptAnimationEvent.h>

TP_THIS_FUNCTION(TActivate, void, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing);
TP_THIS_FUNCTION(TAddInventoryItem, void*, TESObjectREFR, TESBoundObject* apItem, BSExtraDataList* apExtraData, uint32_t aCount, TESObjectREFR* apOldOwner);
TP_THIS_FUNCTION(TRemoveInventoryItem, void*, TESObjectREFR, float* apUnk0, TESBoundObject* apItem, uint32_t aCount, uint32_t aUnk1, BSExtraDataList* apExtraData, TESObjectREFR* apNewOwner, NiPoint3* apUnk2, NiPoint3* apUnk3);
TP_THIS_FUNCTION(TPlayAnimationAndWait, bool, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apAnimation, BSFixedString* apEventName);
TP_THIS_FUNCTION(TPlayAnimation, bool, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apEventName);

static TActivate* RealActivate = nullptr;
static TAddInventoryItem* RealAddInventoryItem = nullptr;
static TRemoveInventoryItem* RealRemoveInventoryItem = nullptr;
static TPlayAnimationAndWait* RealPlayAnimationAndWait = nullptr;
static TPlayAnimation* RealPlayAnimation = nullptr;

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

    POINTER_SKYRIMSE(TGetContainterChanges, s_getContainerChangs, 16040);
    
    return ThisCall(s_getContainerChangs, this);
}

void TESObjectREFR::SaveInventory(BGSSaveFormBuffer* apBuffer) const noexcept
{
    auto changes = GetContainerChanges();

    auto entries = changes->entries;
    uint32_t entryCount = 0;
    for (auto entry : *entries)
    {
        entryCount++;
    }
    if (entryCount > 1024)
        spdlog::error("Inventory entry count is really big: {:X}:{}", formID, entryCount);

    changes->Save(apBuffer);
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

TESContainer* TESObjectREFR::GetContainer() const noexcept
{
    TP_THIS_FUNCTION(TGetContainer, TESContainer*, const TESObjectREFR);

    POINTER_SKYRIMSE(TGetContainer, s_getContainer, 19702);

    return ThisCall(s_getContainer, this);
}

int64_t TESObjectREFR::GetItemCountInInventory(TESForm* apItem) const noexcept
{
    int64_t count = GetContainer()->GetItemCount(apItem);

    auto* pContainerChanges = GetContainerChanges()->entries;
    for (auto pChange : *pContainerChanges)
    {
        if (pChange && pChange->form)
        {
            if (pChange->form->formID == apItem->formID)
            {
                count += pChange->count;
                break;
            }
        }
    }

    return count;
}

void TESObjectREFR::Activate(TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* aObjectToGet, int32_t aCount, char aDefaultProcessing) noexcept
{
    return ThisCall(RealActivate, this, apActivator, aUnk1, aObjectToGet, aCount, aDefaultProcessing);
}

void TESObjectREFR::EnableImpl() noexcept
{
    TP_THIS_FUNCTION(TEnableImpl, void, TESObjectREFR, bool aResetInventory);

    POINTER_SKYRIMSE(TEnableImpl, s_enable, 19800);
    
    ThisCall(s_enable, this, false);
}

static thread_local bool s_cancelAnimationWaitEvent = false;

bool TESObjectREFR::PlayAnimationAndWait(BSFixedString* apAnimation, BSFixedString* apEventName) noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(bool, ObjectReference, PlayAnimationAndWait, BSFixedString*, BSFixedString*);

    s_cancelAnimationWaitEvent = true;
    bool result = s_pPlayAnimationAndWait(this, apAnimation, apEventName);
    s_cancelAnimationWaitEvent = false;
    return result;
}

bool TP_MAKE_THISCALL(HookPlayAnimationAndWait, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apAnimation, BSFixedString* apEventName)
{
    spdlog::debug("Animation: {}, EventName: {}", apAnimation->AsAscii(), apEventName->AsAscii());

    if (!s_cancelAnimationWaitEvent && (apSelf->formID < 0xFF000000))
        World::Get().GetRunner().Trigger(ScriptAnimationEvent(apSelf->formID, apAnimation->AsAscii(), apEventName->AsAscii()));

    return ThisCall(RealPlayAnimationAndWait, apThis, auiStackID, apSelf, apAnimation, apEventName);
}

static thread_local bool s_cancelAnimationEvent = false;

bool TESObjectREFR::PlayAnimation(BSFixedString* apEventName) noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(bool, ObjectReference, PlayAnimation, BSFixedString*);

    s_cancelAnimationEvent = true;
    bool result = s_pPlayAnimation(this, apEventName);
    s_cancelAnimationEvent = false;
    return result;
}

bool TP_MAKE_THISCALL(HookPlayAnimation, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apEventName)
{
    spdlog::debug("EventName: {}", apEventName->AsAscii());

    if (!s_cancelAnimationEvent && (apSelf->formID < 0xFF000000))
        World::Get().GetRunner().Trigger(ScriptAnimationEvent(apSelf->formID, String{}, apEventName->AsAscii()));

    return ThisCall(RealPlayAnimation, apThis, auiStackID, apSelf, apEventName);
}

void TP_MAKE_THISCALL(HookActivate, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing)
{
    auto* pActivator = RTTI_CAST(apActivator, TESObjectREFR, Actor);
    if (pActivator)
        World::Get().GetRunner().Trigger(ActivateEvent(apThis, pActivator, apObjectToGet, aUnk1, aCount, aDefaultProcessing));

    return ThisCall(RealActivate, apThis, apActivator, aUnk1, apObjectToGet, aCount, aDefaultProcessing);
}

void* TP_MAKE_THISCALL(HookAddInventoryItem, TESObjectREFR, TESBoundObject* apItem, BSExtraDataList* apExtraData, uint32_t aCount, TESObjectREFR* apOldOwner)
{
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    return ThisCall(RealAddInventoryItem, apThis, apItem, apExtraData, aCount, apOldOwner);
}

// TODO: here's your deadlock/memory leak, fix that
void* TP_MAKE_THISCALL(HookRemoveInventoryItem, TESObjectREFR, float* apUnk0, TESBoundObject* apItem, uint32_t aCount, uint32_t aUnk1, BSExtraDataList* apExtraData, TESObjectREFR* apNewOwner, NiPoint3* apUnk2, NiPoint3* apUnk3)
{
    thread_local static uint32_t count = 0;
    count++;
    if (count > 1)
        spdlog::error("\tRecursive RemoveInventoryItem!");

    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));

    auto result = ThisCall(RealRemoveInventoryItem, apThis, apUnk0, apItem, aCount, aUnk1, apExtraData, apNewOwner, apUnk2, apUnk3);

    count--;

    return result;
}

static TiltedPhoques::Initializer s_objectReferencesHooks([]() {
    POINTER_SKYRIMSE(TActivate, s_activate, 19796);
    POINTER_SKYRIMSE(TAddInventoryItem, s_addInventoryItem, 19708);
    POINTER_SKYRIMSE(TRemoveInventoryItem, s_removeInventoryItem, 19689);
    POINTER_SKYRIMSE(TPlayAnimationAndWait, s_playAnimationAndWait, 56206);
    POINTER_SKYRIMSE(TPlayAnimation, s_playAnimation, 56205);

    RealActivate = s_activate.Get();
    RealAddInventoryItem = s_addInventoryItem.Get();
    RealRemoveInventoryItem = s_removeInventoryItem.Get();
    RealPlayAnimationAndWait = s_playAnimationAndWait.Get();
    RealPlayAnimation = s_playAnimation.Get();

    TP_HOOK(&RealActivate, HookActivate);
    TP_HOOK(&RealAddInventoryItem, HookAddInventoryItem);
    TP_HOOK(&RealRemoveInventoryItem, HookRemoveInventoryItem);
    TP_HOOK(&RealPlayAnimationAndWait, HookPlayAnimationAndWait);
    TP_HOOK(&RealPlayAnimation, HookPlayAnimation);
});
