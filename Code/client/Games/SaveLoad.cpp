#include <TiltedOnlinePCH.h>

#include <SaveLoad.h>
#include <Games/Overrides.h>

#include <Forms/TESForm.h>

#include <World.h>

#include <TiltedCore/Serialization.hpp>
#include <TiltedCore/Buffer.hpp>

using TiltedPhoques::Serialization;
using TiltedPhoques::ViewBuffer;

#if TP_FALLOUT4
BGSSaveLoadManager* BGSSaveLoadManager::Get() noexcept
{
    POINTER_FALLOUT4(BGSSaveLoadManager*, instance, 1247321);

    return *(instance.Get());
}
#endif

BGSSaveFormBuffer::BGSSaveFormBuffer()
{
    TP_THIS_FUNCTION(CtorT, BGSSaveFormBuffer*, BGSSaveFormBuffer);

    POINTER_SKYRIMSE(CtorT, ctor, 36035);
    POINTER_FALLOUT4(CtorT, ctor, 824899);

    ThisCall(ctor, this);

    position = 0;
}


void BGSSaveFormBuffer::WriteId(uint32_t aId) noexcept
{
    uint32_t modId = 0;
    uint32_t baseId = 0;

    World::Get().GetModSystem().GetServerModId(aId, modId, baseId);

    auto pWriteLocation = reinterpret_cast<uint8_t*>(buffer + position);

    ViewBuffer view(pWriteLocation, capacity - position);
    Buffer::Writer writer(&view);

    Serialization::WriteVarInt(writer, modId);
    Serialization::WriteVarInt(writer, baseId);

    position += writer.Size() & 0xFFFFFFFF;
}


BGSLoadFormBuffer::BGSLoadFormBuffer(const uint32_t aChangeFlags)
{
    TP_THIS_FUNCTION(CtorT, BGSLoadFormBuffer*, BGSLoadFormBuffer);

    POINTER_SKYRIMSE(CtorT, ctor, 35993);
    POINTER_FALLOUT4(CtorT, ctor, 994876);

    ThisCall(ctor, this);

    changeFlags = aChangeFlags;
    loadFlag = 0x40;
    position = 0;
    maybeMoreFlags = 0;

#if TP_PLATFORM_64
#   if TP_FALLOUT4
    unk1C = 1;
#   else
    unk1C = -1;
#   endif
#endif
}

TP_THIS_FUNCTION(TBGSLoadFormBuffer_ReadFormId, bool, BGSLoadFormBuffer, uint32_t&);
TP_THIS_FUNCTION(TBGSSaveFormBuffer_WriteFormId, void, BGSSaveFormBuffer, TESForm*);
TP_THIS_FUNCTION(TBGSSaveFormBuffer_WriteId, void, BGSSaveFormBuffer, uint64_t);

static TBGSSaveFormBuffer_WriteFormId* RealBGSSaveFormBuffer_WriteFormId = nullptr;
static TBGSLoadFormBuffer_ReadFormId* RealBGSLoadFormBuffer_ReadFormId = nullptr;
static TBGSSaveFormBuffer_WriteId* RealBGSSaveFormBuffer_WriteId = nullptr;

void TP_MAKE_THISCALL(BGSSaveFormBuffer_WriteFormId, BGSSaveFormBuffer, TESForm* apForm)
{
    if (!ScopedSaveLoadOverride::IsOverriden())
    {
        ThisCall(RealBGSSaveFormBuffer_WriteFormId, apThis, apForm);
        return;
    }

    apThis->WriteId(apForm ? apForm->formID : 0);
}

void TP_MAKE_THISCALL(BGSSaveFormBuffer_WriteId, BGSSaveFormBuffer, uint64_t aId)
{
    if (!ScopedSaveLoadOverride::IsOverriden())
    {
        ThisCall(RealBGSSaveFormBuffer_WriteId, apThis, aId);
        return;
    }

    apThis->WriteId(aId & 0xFFFFFFFF);
}


bool TP_MAKE_THISCALL(BGSLoadFormBuffer_LoadFormId, BGSLoadFormBuffer, uint32_t& aFormId)
{
    if (!ScopedSaveLoadOverride::IsOverriden())
    {
        return ThisCall(RealBGSLoadFormBuffer_ReadFormId, apThis, aFormId);
    }

    uint8_t* pReadLocation = (uint8_t*)(apThis->buffer + apThis->position);

    ViewBuffer buffer(pReadLocation, apThis->capacity - apThis->position);
    ViewBuffer::Reader reader(&buffer);

    const uint32_t modId = Serialization::ReadVarInt(reader) & 0xFFFFFFFF;
    const uint32_t baseId = Serialization::ReadVarInt(reader) & 0xFFFFFFFF;

    aFormId = 0;

    if(modId != 0 || baseId != 0)
        aFormId = World::Get().GetModSystem().GetGameId(modId, baseId);

    apThis->position += reader.Size() & 0xFFFFFFFF;

    return true;
}

static TiltedPhoques::Initializer s_saveLoadHooks([]()
    {
        POINTER_FALLOUT4(TBGSLoadFormBuffer_ReadFormId, s_readFormId, 601669);
        POINTER_SKYRIMSE(TBGSLoadFormBuffer_ReadFormId, s_readFormId, 36000);

        POINTER_FALLOUT4(TBGSSaveFormBuffer_WriteFormId, s_writeFormId, 556002);
        POINTER_SKYRIMSE(TBGSSaveFormBuffer_WriteFormId, s_writeFormId, 36048);

        POINTER_FALLOUT4(TBGSSaveFormBuffer_WriteId, s_writeId, 210459);
        POINTER_SKYRIMSE(TBGSSaveFormBuffer_WriteId, s_writeId, 36047);

        RealBGSLoadFormBuffer_ReadFormId = s_readFormId.Get();
        RealBGSSaveFormBuffer_WriteFormId = s_writeFormId.Get();
        RealBGSSaveFormBuffer_WriteId = s_writeId.Get();

        TP_HOOK(&RealBGSLoadFormBuffer_ReadFormId, BGSLoadFormBuffer_LoadFormId);
        TP_HOOK(&RealBGSSaveFormBuffer_WriteFormId, BGSSaveFormBuffer_WriteFormId);
        TP_HOOK(&RealBGSSaveFormBuffer_WriteId, BGSSaveFormBuffer_WriteId);
    });
