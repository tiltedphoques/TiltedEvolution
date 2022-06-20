#include "BGSCreatedObjectManager.h"

// TODO: ft
BGSCreatedObjectManager* BGSCreatedObjectManager::Get() noexcept
{
#if TP_SKYRIM64
    POINTER_SKYRIMSE(BGSCreatedObjectManager*, pObjManager, 400320);
    return *pObjManager.Get();
#else
    return nullptr;
#endif
}
