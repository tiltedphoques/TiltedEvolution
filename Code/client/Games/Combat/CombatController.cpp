#include "CombatController.h"

// TODO: ft
void CombatController::SetTarget(Actor* apTarget)
{
#if TP_SKYRIM64
    TP_THIS_FUNCTION(TSetTarget, void, CombatController, Actor*);
    POINTER_SKYRIMSE(TSetTarget, s_setTarget, 33235);
    TiltedPhoques::ThisCall(s_setTarget, this, apTarget);
#endif
}

