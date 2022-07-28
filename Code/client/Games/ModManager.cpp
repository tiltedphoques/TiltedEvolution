#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

#include <Actor.h>

#if TP_FALLOUT
#include <Misc/NEW_REFR_DATA.h>
#endif

ModManager* ModManager::Get() noexcept
{
    POINTER_FALLOUT4(ModManager*, modManager, 711559);
    POINTER_SKYRIMSE(ModManager*, modManager, 400269);
    
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


#if TP_FALLOUT4

TP_THIS_FUNCTION(TSpawnNewREFR, uint32_t&, ModManager, uint32_t& refHandle, NEW_REFR_DATA* apData);
TSpawnNewREFR* RealSpawnNewREFR;

uint32_t& TP_MAKE_THISCALL(SpawnNewREFR, ModManager, uint32_t& refHandle, NEW_REFR_DATA* apData)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    return RealSpawnNewREFR(apThis, refHandle, apData);
}

#else

TP_THIS_FUNCTION(TSpawnNewREFR, uint32_t&, ModManager, uint32_t& aRefHandleOut, TESForm* apBaseForm, NiPoint3* apPosition, NiPoint3* apRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apActor, uintptr_t a9, uintptr_t a10, char aForcePersist, char a12);
TSpawnNewREFR* RealSpawnNewREFR;

uint32_t& TP_MAKE_THISCALL(SpawnNewREFR, ModManager, uint32_t& aRefHandleOut, TESForm* apBaseForm, NiPoint3* apPosition, NiPoint3* apRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apActor, uintptr_t a9, uintptr_t a10, char aForcePersist, char a12)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    return ThisCall(RealSpawnNewREFR, apThis, aRefHandleOut, apBaseForm, apPosition, apRotation, apParentCell, apWorldSpace, apActor, a9, a10, aForcePersist, a12);
}
#endif

uint32_t ModManager::Spawn(NiPoint3& aPosition, NiPoint3& aRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apCharacter) noexcept
{
    uint32_t refrHandle = 0;

#if TP_FALLOUT4
    NEW_REFR_DATA refrData;
    refrData.postion = aPosition;
    refrData.rotation = aRotation;
    refrData.baseForm = apCharacter->baseForm;
    refrData.refrToPlace = apCharacter;
    refrData.parentCell = apParentCell;
    refrData.worldspace = apWorldSpace;

    ThisCall(RealSpawnNewREFR, this, refrHandle, &refrData);
#else
    ThisCall(RealSpawnNewREFR, this, refrHandle, apCharacter->baseForm, &aPosition, &aRotation, apParentCell, apWorldSpace, apCharacter, 0, 0, static_cast<char>(0), static_cast<char>(1));
#endif

    return refrHandle;
}

Mod* ModManager::GetByName(const char* acpName) const noexcept
{
    auto pEntry = &mods.entry;

    while(pEntry && pEntry->data)
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
    POINTER_SKYRIMSE(TModManager, getCell, 13718);
    POINTER_FALLOUT4(TModManager, getCell, 385618);

    return ThisCall(getCell, this, aX, aY, aWorldSpace, aSpawnCell);
}

static TiltedPhoques::Initializer s_tesHooks([]()
{
    POINTER_FALLOUT4(TSpawnNewREFR, s_realSpawnNewREFR, 500305);
    POINTER_SKYRIMSE(TSpawnNewREFR, s_realSpawnNewREFR, 13723);

    RealSpawnNewREFR = s_realSpawnNewREFR.Get();

    //TP_HOOK(&RealSpawnNewREFR, SpawnNewREFR);
});

