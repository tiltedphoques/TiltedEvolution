#include "BGSCreatedObjectManager.h"

BGSCreatedObjectManager* BGSCreatedObjectManager::Get() noexcept
{
    POINTER_SKYRIMSE(BGSCreatedObjectManager*, pObjManager, 400320);
    POINTER_FALLOUT4(BGSCreatedObjectManager*, pObjManager, 1000679);
    return *pObjManager.Get();
}
