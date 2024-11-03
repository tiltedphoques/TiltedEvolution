#include "ActorState.h"

bool ActorState::SetWeaponDrawn(bool aDraw) noexcept
{
    TP_THIS_FUNCTION(TSetWeaponState, bool, ActorState, bool aDraw);
    POINTER_SKYRIMSE(TSetWeaponState, setWeaponState, 38979);

    return TiltedPhoques::ThisCall(setWeaponState, this, aDraw);
}
