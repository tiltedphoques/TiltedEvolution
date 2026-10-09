#include "BGSCreatedObjectManager.h"

BGSCreatedObjectManager* BGSCreatedObjectManager::Get() noexcept
{
    POINTER_GAME(BGSCreatedObjectManager*, pObjManager, 400320, 4796296);
    return *pObjManager.Get();
}
