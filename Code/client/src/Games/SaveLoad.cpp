#include <Games/SaveLoad.h>

#include <Games/Skyrim/Forms/TESForm.h>
#include <Games/Fallout4/Forms/TESForm.h>

#include <World.h>

#if TP_FALLOUT4
BGSSaveLoadManager* BGSSaveLoadManager::Get() noexcept
{
    POINTER_FALLOUT4(BGSSaveLoadManager*, instance, 0x145A735C8 - 0x140000000);

    return *(instance.Get());
}
#endif

BGSSaveFormBuffer::BGSSaveFormBuffer()
{
    TP_THIS_FUNCTION(CtorT, BGSSaveFormBuffer*, BGSSaveFormBuffer);

    POINTER_SKYRIMSE(CtorT, ctor, 0x140599440 - 0x140000000);
    POINTER_FALLOUT4(CtorT, ctor, 0x140D09300 - 0x140000000);

    ThisCall(ctor, this);

    position = 0;
}

BGSLoadFormBuffer::BGSLoadFormBuffer(const uint32_t aChangeFlags)
{
    TP_THIS_FUNCTION(CtorT, BGSLoadFormBuffer*, BGSLoadFormBuffer);

    POINTER_SKYRIMSE(CtorT, ctor, 0x140597870 - 0x140000000);
    POINTER_FALLOUT4(CtorT, ctor, 0x140123F00 - 0x140000000);

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

thread_local bool g_overrideFormId = false;

ScopedSaveLoadOverride::ScopedSaveLoadOverride()
{
    if(World::Get().GetTransport().IsOnline())
        g_overrideFormId = true;
}

ScopedSaveLoadOverride::~ScopedSaveLoadOverride()
{
    g_overrideFormId = false;
}

TP_THIS_FUNCTION(TBGSLoadFormBuffer_ReadFormId, bool, BGSLoadFormBuffer, uint32_t&);
TP_THIS_FUNCTION(TBGSSaveFormBuffer_WriteFormId, void, BGSSaveFormBuffer, TESForm*);

static TBGSSaveFormBuffer_WriteFormId* RealBGSSaveFormBuffer_WriteFormId = nullptr;
static TBGSLoadFormBuffer_ReadFormId* RealBGSLoadFormBuffer_ReadFormId = nullptr;

void TP_MAKE_THISCALL(BGSSaveFormBuffer_WriteFormId, BGSSaveFormBuffer, TESForm* apForm)
{
    if (!g_overrideFormId)
    {

        ThisCall(RealBGSSaveFormBuffer_WriteFormId, apThis, apForm);
        return;
    }

    uint32_t modId = 0;
    uint32_t baseId = 0;

    if (apForm)
    {
        World::Get().GetModSystem().GetServerModId(apForm->formID, modId, baseId);
    }

    const auto pWriteLocation = reinterpret_cast<uint32_t*>(apThis->buffer + apThis->position);
    pWriteLocation[0] = modId;
    pWriteLocation[1] = baseId;

    apThis->position += 8;
}

bool TP_MAKE_THISCALL(BGSLoadFormBuffer_LoadFormId, BGSLoadFormBuffer, uint32_t& aFormId)
{
    if (!g_overrideFormId)
    {
        return ThisCall(RealBGSLoadFormBuffer_ReadFormId, apThis, aFormId);
    }

    const auto pReadLocation = reinterpret_cast<const uint32_t*>(apThis->buffer + apThis->position);

    const uint32_t modId = pReadLocation[0];
    const uint32_t baseId = pReadLocation[1];

    aFormId = 0;

    if(modId != 0 || baseId != 0)
        aFormId = World::Get().GetModSystem().GetGameId(modId, baseId);

    apThis->position += 8;

    return true;
}

static TiltedPhoques::Initializer s_saveLoadHooks([]()
    {
        POINTER_FALLOUT4(TBGSLoadFormBuffer_ReadFormId, s_readFormId, 0x140D06BC0 - 0x140000000);
        POINTER_SKYRIMSE(TBGSLoadFormBuffer_ReadFormId, s_readFormId, 0x140597AA0 - 0x140000000);

        POINTER_FALLOUT4(TBGSSaveFormBuffer_WriteFormId, s_writeFormId, 0x140D09AA0 - 0x140000000);
        POINTER_SKYRIMSE(TBGSSaveFormBuffer_WriteFormId, s_writeFormId, 0x1405999B0 - 0x140000000);

        RealBGSLoadFormBuffer_ReadFormId = s_readFormId.Get();
        RealBGSSaveFormBuffer_WriteFormId = s_writeFormId.Get();

        TP_HOOK(&RealBGSLoadFormBuffer_ReadFormId, BGSLoadFormBuffer_LoadFormId);
        TP_HOOK(&RealBGSSaveFormBuffer_WriteFormId, BGSSaveFormBuffer_WriteFormId);
    });
