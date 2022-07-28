#include <TiltedOnlinePCH.h>

#include <PlayerCharacter.h>
#include <Forms/BGSHeadPart.h>
#include <Forms/TESNPC.h>

#include <Games/IFormFactory.h>

#include <SaveLoad.h>
#include <Games/Overrides.h>


TESForm* TESForm::GetById(const uint32_t aId)
{
    using TGetFormById = TESForm*(uint32_t);

    POINTER_FALLOUT4(TGetFormById, getFormById, 796115);
    POINTER_SKYRIMSE(TGetFormById, getFormById, 14617);

    return getFormById.Get()(aId);
}

void TESNPC::Serialize(String* apSaveBuffer) const noexcept
{
    ScopedSaveLoadOverride saveLoadOverride;

    char buffer[1 << 15];

    BGSSaveFormBuffer saveBuffer;
    saveBuffer.buffer = buffer;
    saveBuffer.capacity = 1 << 15;
    saveBuffer.changeFlags = GetChangeFlags();

    Save(&saveBuffer);

    apSaveBuffer->assign(saveBuffer.buffer, saveBuffer.position);

    saveBuffer.buffer = nullptr;
}

void TESNPC::Deserialize(const String& acBuffer, uint32_t aChangeFlags) noexcept
{
    ScopedSaveLoadOverride saveLoadOverride;

    BGSLoadFormBuffer loadBuffer(aChangeFlags);
    loadBuffer.SetSize(acBuffer.size() & 0xFFFFFFFF);
    loadBuffer.buffer = acBuffer.data();
    loadBuffer.formId = formID;
    loadBuffer.form = this;

    Load(&loadBuffer);

    loadBuffer.buffer = nullptr;
}

void TESNPC::Initialize() noexcept
{
    auto pPlayerBaseForm = Cast<TESNPC>(PlayerCharacter::Get()->baseForm);

    // These values are all defaulted, if the other actor did not modify them they won't be loaded, therefore we need to force them before load
    attackDataForm.attackDataMap = pPlayerBaseForm->attackDataForm.attackDataMap;
    npcClass = pPlayerBaseForm->npcClass;
    combatStyle = pPlayerBaseForm->combatStyle;
    raceForm.race = pPlayerBaseForm->raceForm.race;
    outfits[0] = pPlayerBaseForm->outfits[0];
#if TP_SKYRIM
    spellList.Initialize();
#endif
    // End defaults

    flags |= 0x200000;
}

void TESForm::Save_Reversed(const uint32_t aChangeFlags, Buffer::Writer& aWriter)
{
    if (aChangeFlags & 1)
    {
        aWriter.WriteBytes(reinterpret_cast<uint8_t*>(&flags), 4);
        aWriter.WriteBytes(reinterpret_cast<uint8_t*>(&unk10), 2);
    }
}

void TESForm::SetSkipSaveFlag(bool aSet) noexcept
{
    if(aSet)
    {
        unk10 = 0xFFFF;
    }
    /*const uint32_t flag = 1 << 14;

    if (aSet)
        flags |= flag;
    else
        flags &= ~flag;*/
}

uint32_t TESForm::GetChangeFlags() const noexcept
{
    struct Unk
    {
#if TP_SKYRIM64
        uint8_t unk0[0x330];
#elif TP_FALLOUT4
        uint8_t unk0[0x350];
#endif
        void* unk330;
    };

    TP_THIS_FUNCTION(InternalGetChangeFlags, bool, void, uint32_t formId, ChangeFlags& changeFlags);

    POINTER_FALLOUT4(InternalGetChangeFlags, internalGetChangeFlags, 1464380);
    POINTER_SKYRIMSE(InternalGetChangeFlags, internalGetChangeFlags, 35503);

    POINTER_FALLOUT4(Unk*, s_singleton, 177948);
    POINTER_SKYRIMSE(Unk*, s_singleton, 403330);

    const auto pUnk = *(s_singleton.Get());
    
    ChangeFlags changeFlags;
    const auto cResult = ThisCall(internalGetChangeFlags, pUnk->unk330, formID, changeFlags);
    if (!cResult)
        return 0;

	return changeFlags.flags;
}

TESNPC* TESNPC::Create(const String& acBuffer, const uint32_t aChangeFlags) noexcept
{
    auto pNpc = IFormFactory::Create<TESNPC>();

    pNpc->Initialize();
    pNpc->Deserialize(acBuffer, aChangeFlags);

    // This forces facegen for some reason
#if TP_SKYRIM
    pNpc->overlayRace = nullptr;
#endif

    return pNpc;
}

BGSHeadPart* TESNPC::GetHeadPart(uint32_t aType)
{
#ifdef TP_SKYRIM
    if (headparts)
    {
        for (auto i = 0; i < headpartsCount; ++i)
        {
            if (headparts[i] && headparts[i]->type == aType)
                return headparts[i];
        }
    }
#endif

    return nullptr;
}
