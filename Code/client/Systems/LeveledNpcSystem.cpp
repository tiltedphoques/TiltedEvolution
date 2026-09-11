#include <TiltedOnlinePCH.h>

#include <Systems/LeveledNpcSystem.h>
#include <Forms/TESNPC.h>

bool LeveledNpcSystem::IsUnresolvedLeveledShell(const TESNPC* apBase) noexcept
{
    if (!apBase || apBase->IsTemporary())
        return false;

    const TESNPC* pTemplate = apBase->faceNPC;
    return pTemplate && pTemplate->formType == FormType::LeveledCharacter;
}
