#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

#include <Actor.h>

ModManager* ModManager::Get() noexcept
{
    POINTER_GAME(ModManager*, modManager, 400269, 4796135);

    return *modManager.Get();
}

uint32_t Mod::GetFormId(uint32_t aBaseId) const noexcept
{
    if (IsLite())
    {
        aBaseId &= 0xFFF;
        aBaseId |= 0xFE000000;
        aBaseId |= uint32_t(liteId) << 12;
    }
    else
    {
        aBaseId &= 0xFFFFFF;
        aBaseId |= uint32_t(standardId) << 24;
    }

    return aBaseId;
}

#if defined(TP_FALLOUT4)
namespace
{
struct NewReferenceData
{
    virtual void HandlePre3D(TESObjectREFR*) {}

    NiPoint3 location;
    NiPoint3 direction;
    TESBoundObject* object = nullptr;
    TESObjectCELL* interior = nullptr;
    TESWorldSpace* world = nullptr;
    TESObjectREFR* reference = nullptr;
    void* primitive = nullptr;
    void* additionalData = nullptr;
    void* extra = nullptr;
    void* instanceFilter = nullptr;
    void* modExtra = nullptr;
    uint16_t maxLevel = 0;
    bool forcePersist = false;
    bool clearStillLoadingFlag = true;
    bool initializeScripts = true;
    bool initiallyDisabled = false;
};

static_assert(offsetof(NewReferenceData, object) == 0x20);
static_assert(offsetof(NewReferenceData, reference) == 0x38);
static_assert(offsetof(NewReferenceData, initializeScripts) == 0x6C);
static_assert(sizeof(NewReferenceData) == 0x70);
}

uint32_t ModManager::Spawn(NiPoint3& aPosition, NiPoint3& aRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, TESBoundObject* apBaseForm) noexcept
{
    NewReferenceData data;
    data.location = aPosition;
    data.direction = aRotation;
    data.object = apBaseForm;
    data.interior = apParentCell;
    data.world = apWorldSpace;

    TP_THIS_FUNCTION(TCreateReference, uint32_t*, ModManager, uint32_t*, NewReferenceData&);
    static VersionDbPtr<TCreateReference> createReference(2192301);
    uint32_t handle = 0;
    TiltedPhoques::ThisCall(createReference, this, &handle, data);
    return handle;
}
#else
TP_THIS_FUNCTION(TSpawnNewREFR, uint32_t&, ModManager, uint32_t& aRefHandleOut, TESForm* apBaseForm, NiPoint3* apPosition, NiPoint3* apRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apActor, uintptr_t a9, uintptr_t a10, char aForcePersist, char a12);
TSpawnNewREFR* RealSpawnNewREFR;

uint32_t& TP_MAKE_THISCALL(SpawnNewREFR, ModManager, uint32_t& aRefHandleOut, TESForm* apBaseForm, NiPoint3* apPosition, NiPoint3* apRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apActor, uintptr_t a9, uintptr_t a10, char aForcePersist, char a12)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    return TiltedPhoques::ThisCall(RealSpawnNewREFR, apThis, aRefHandleOut, apBaseForm, apPosition, apRotation, apParentCell, apWorldSpace, apActor, a9, a10, aForcePersist, a12);
}

uint32_t ModManager::Spawn(NiPoint3& aPosition, NiPoint3& aRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apCharacter) noexcept
{
    uint32_t refrHandle = 0;

    TiltedPhoques::ThisCall(RealSpawnNewREFR, this, refrHandle, apCharacter->baseForm, &aPosition, &aRotation, apParentCell, apWorldSpace, apCharacter, 0, 0, static_cast<char>(0), static_cast<char>(1));

    return refrHandle;
}
#endif

Mod* ModManager::GetByName(const char* acpName) const noexcept
{
    auto pEntry = &mods.entry;

    while (pEntry && pEntry->data)
    {
        if (_stricmp(acpName, pEntry->data->filename) == 0)
            return pEntry->data;

        pEntry = pEntry->next;
    }

    return nullptr;
}

TESObjectCELL* ModManager::GetCellFromCoordinates(int32_t aX, int32_t aY, TESWorldSpace* aWorldSpace, bool aSpawnCell) noexcept
{
    TP_THIS_FUNCTION(TModManager, TESObjectCELL*, ModManager, int32_t, int32_t, TESWorldSpace*, bool);
    POINTER_GAME(TModManager, getCell, 13718, 2192295);

    return TiltedPhoques::ThisCall(getCell, this, aX, aY, aWorldSpace, aSpawnCell);
}

#if !defined(TP_FALLOUT4)
static TiltedPhoques::Initializer s_tesHooks(
    []()
    {
        POINTER_SKYRIMSE(TSpawnNewREFR, s_realSpawnNewREFR, 13723);

        RealSpawnNewREFR = s_realSpawnNewREFR.Get();

        // TP_HOOK(&RealSpawnNewREFR, SpawnNewREFR);
    });
#endif
