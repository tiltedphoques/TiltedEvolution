#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

#include <Actor.h>

ModManager* ModManager::Get() noexcept
{
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
    POINTER_SKYRIMSE(TModManager, getCell, 13718);

    return TiltedPhoques::ThisCall(getCell, this, aX, aY, aWorldSpace, aSpawnCell);
}

static TiltedPhoques::Initializer s_tesHooks(
    []()
    {
        POINTER_SKYRIMSE(TSpawnNewREFR, s_realSpawnNewREFR, 13723);

        RealSpawnNewREFR = s_realSpawnNewREFR.Get();

        // TP_HOOK(&RealSpawnNewREFR, SpawnNewREFR);
    });
