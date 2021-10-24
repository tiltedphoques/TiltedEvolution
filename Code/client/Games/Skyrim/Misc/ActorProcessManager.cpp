#include "ActorProcessManager.h"

bool ActorProcessManager::SetCurrentAmmo(TESAmmo* apAmmo) noexcept
{
    TP_THIS_FUNCTION(TSetAmmo, bool, ActorProcessManager, TESAmmo* apAmmo);

    POINTER_SKYRIMSE(TSetAmmo, s_setAmmo, 0x14067AA50 - 0x140000000);

    return ThisCall(s_setAmmo, this, apAmmo);
}
