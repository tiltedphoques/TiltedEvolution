#include <stdafx.h>

#include <Games/References.h>
#include <Games/Overrides.h>

#include <World.h>
#include <Services/PapyrusService.h>

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
