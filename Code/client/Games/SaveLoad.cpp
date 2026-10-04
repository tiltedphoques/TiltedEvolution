#include <TiltedOnlinePCH.h>

#include <SaveLoad.h>
#include <Games/Overrides.h>

#include <Forms/TESForm.h>

#include <World.h>

#include <TiltedCore/Serialization.hpp>
#include <TiltedCore/Buffer.hpp>

using TiltedPhoques::Serialization;
using TiltedPhoques::ViewBuffer;

BGSSaveFormBuffer::BGSSaveFormBuffer()
{
    TP_THIS_FUNCTION(CtorT, BGSSaveFormBuffer*, BGSSaveFormBuffer);

    POINTER_GAME(CtorT, ctor, 36035, 2228301);

    TiltedPhoques::ThisCall(ctor, this);

    position = 0;
#ifdef TP_FALLOUT4
    using TGetVersion = uint8_t();
    static VersionDbPtr<TGetVersion> getVersion(2228051);
    version = getVersion.Get()();
#endif
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
#ifdef TP_FALLOUT4
    TP_THIS_FUNCTION(TConstructGameBuffer, BGSLoadFormBuffer*, BGSLoadFormBuffer);
    TP_THIS_FUNCTION(TConstructFormData, void*, void);
    static VersionDbPtr<TConstructGameBuffer> constructGameBuffer(2228258);
    static VersionDbPtr<TConstructFormData> constructFormData(2228249);
    static VersionDbPtr<void> vtable(950365);
    TiltedPhoques::ThisCall(constructGameBuffer, this);
    TiltedPhoques::ThisCall(constructFormData, reinterpret_cast<uint8_t*>(this) + 0x28);
    *reinterpret_cast<void**>(this) = vtable.Get();
#else
    TP_THIS_FUNCTION(CtorT, BGSLoadFormBuffer*, BGSLoadFormBuffer);
    POINTER_SKYRIMSE(CtorT, ctor, 35993);
    TiltedPhoques::ThisCall(ctor, this);
#endif

    changeFlags = aChangeFlags;
#ifdef TP_FALLOUT4
    using TGetVersion = uint8_t();
    static VersionDbPtr<TGetVersion> getVersion(2228051);
    loadFlag = getVersion.Get()();
#else
    loadFlag = 0x40;
#endif
    position = 0;
    maybeMoreFlags = 0;

    unk1C = -1;
}

TP_THIS_FUNCTION(TBGSLoadFormBuffer_ReadFormId, bool, BGSLoadFormBuffer, uint32_t&);
#ifdef TP_FALLOUT4
TP_THIS_FUNCTION(TBGSSaveFormBuffer_WriteFormId, void, BGSSaveFormBuffer, const TESForm*, uint32_t);
TP_THIS_FUNCTION(TBGSSaveFormBuffer_WriteId, void, BGSSaveFormBuffer, uint32_t, uint32_t);
#else
TP_THIS_FUNCTION(TBGSSaveFormBuffer_WriteFormId, void, BGSSaveFormBuffer, TESForm*);
TP_THIS_FUNCTION(TBGSSaveFormBuffer_WriteId, void, BGSSaveFormBuffer, uint64_t);
#endif

static TBGSSaveFormBuffer_WriteFormId* RealBGSSaveFormBuffer_WriteFormId = nullptr;
static TBGSLoadFormBuffer_ReadFormId* RealBGSLoadFormBuffer_ReadFormId = nullptr;
static TBGSSaveFormBuffer_WriteId* RealBGSSaveFormBuffer_WriteId = nullptr;

#ifdef TP_FALLOUT4
void TP_MAKE_THISCALL(BGSSaveFormBuffer_WriteFormId, BGSSaveFormBuffer, const TESForm* apForm, uint32_t aPosition)
#else
void TP_MAKE_THISCALL(BGSSaveFormBuffer_WriteFormId, BGSSaveFormBuffer, TESForm* apForm)
#endif
{
    if (!ScopedSaveLoadOverride::IsOverriden())
    {
#ifdef TP_FALLOUT4
        TiltedPhoques::ThisCall(RealBGSSaveFormBuffer_WriteFormId, apThis, apForm, aPosition);
#else
        TiltedPhoques::ThisCall(RealBGSSaveFormBuffer_WriteFormId, apThis, apForm);
#endif
        return;
    }

    apThis->WriteId(apForm ? apForm->formID : 0);
}

#ifdef TP_FALLOUT4
void TP_MAKE_THISCALL(BGSSaveFormBuffer_WriteId, BGSSaveFormBuffer, uint32_t aId, uint32_t aPosition)
#else
void TP_MAKE_THISCALL(BGSSaveFormBuffer_WriteId, BGSSaveFormBuffer, uint64_t aId)
#endif
{
    if (!ScopedSaveLoadOverride::IsOverriden())
    {
#ifdef TP_FALLOUT4
        TiltedPhoques::ThisCall(RealBGSSaveFormBuffer_WriteId, apThis, aId, aPosition);
#else
        TiltedPhoques::ThisCall(RealBGSSaveFormBuffer_WriteId, apThis, aId);
#endif
        return;
    }

    apThis->WriteId(aId & 0xFFFFFFFF);
}

bool TP_MAKE_THISCALL(BGSLoadFormBuffer_LoadFormId, BGSLoadFormBuffer, uint32_t& aFormId)
{
    if (!ScopedSaveLoadOverride::IsOverriden())
    {
        return TiltedPhoques::ThisCall(RealBGSLoadFormBuffer_ReadFormId, apThis, aFormId);
    }

    uint8_t* pReadLocation = (uint8_t*)(apThis->buffer + apThis->position);

    ViewBuffer buffer(pReadLocation, apThis->capacity - apThis->position);
    ViewBuffer::Reader reader(&buffer);

    const uint32_t modId = Serialization::ReadVarInt(reader) & 0xFFFFFFFF;
    const uint32_t baseId = Serialization::ReadVarInt(reader) & 0xFFFFFFFF;

    aFormId = 0;

    if (modId != 0 || baseId != 0)
        aFormId = World::Get().GetModSystem().GetGameId(modId, baseId);

    apThis->position += reader.Size() & 0xFFFFFFFF;

    return true;
}

static TiltedPhoques::Initializer s_saveLoadHooks(
    []()
    {
        POINTER_GAME(TBGSLoadFormBuffer_ReadFormId, s_readFormId, 36000, 2228265);
        POINTER_GAME(TBGSSaveFormBuffer_WriteFormId, s_writeFormId, 36048, 2228314);
        POINTER_GAME(TBGSSaveFormBuffer_WriteId, s_writeId, 36047, 2228313);

        RealBGSLoadFormBuffer_ReadFormId = s_readFormId.Get();
        RealBGSSaveFormBuffer_WriteFormId = s_writeFormId.Get();
        RealBGSSaveFormBuffer_WriteId = s_writeId.Get();

        TP_HOOK(&RealBGSLoadFormBuffer_ReadFormId, BGSLoadFormBuffer_LoadFormId);
        TP_HOOK(&RealBGSSaveFormBuffer_WriteFormId, BGSSaveFormBuffer_WriteFormId);
        TP_HOOK(&RealBGSSaveFormBuffer_WriteId, BGSSaveFormBuffer_WriteId);
    });
