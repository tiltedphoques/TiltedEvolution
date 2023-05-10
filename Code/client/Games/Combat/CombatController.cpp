#include "CombatController.h"

TP_THIS_FUNCTION(TUpdateTarget, bool, CombatController);
static TUpdateTarget* RealUpdateTarget = nullptr;

bool TP_MAKE_THISCALL(HookUpdateTarget, CombatController)
{
    if (!apThis->startedCombat)
        return TiltedPhoques::ThisCall(RealUpdateTarget, apThis);

    return false;
}

TP_THIS_FUNCTION(TSetTarget, void, CombatController, Actor*);
static TSetTarget* RealSetTarget = nullptr;

void TP_MAKE_THISCALL(HookSetTarget, CombatController, Actor* apTarget)
{
    spdlog::info("In game calls SetTarget, Form id: {:X}", apTarget->formID);
    TiltedPhoques::ThisCall(RealSetTarget, apThis, apTarget);
}

// TODO: ft
void CombatController::SetTarget(Actor* apTarget)
{
#if TP_SKYRIM64
    spdlog::info("We, the client, call SetTarget, Form id: {:X}", apTarget->formID);
    TiltedPhoques::ThisCall(RealSetTarget, this, apTarget);
#endif
}

static TiltedPhoques::Initializer s_combatControllerHooks(
    []()
    {
        POINTER_SKYRIMSE(TSetTarget, s_setTarget, 33235);
        POINTER_SKYRIMSE(TUpdateTarget, s_updateTarget, 33236);

        RealSetTarget = s_setTarget.Get();
        RealUpdateTarget = s_updateTarget.Get();

        TP_HOOK(&RealSetTarget, HookSetTarget);
        TP_HOOK(&RealUpdateTarget, HookUpdateTarget);
    });

