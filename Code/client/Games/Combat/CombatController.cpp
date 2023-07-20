#include "CombatController.h"
#include "CombatTargetSelector.h"

// TODO: ft

void ArrayQuickSortRecursiveCombatTargets(GameArray<CombatTargetSelector*>* apArray, uint32_t aUnk3, uint32_t aUnk4)
{
    using TArrayQuickSort = void(GameArray<CombatTargetSelector*>* apArray, void* apFunction, uint32_t aUnk3, uint32_t aUnk4);
    POINTER_SKYRIMSE(TArrayQuickSort, arrayQuickSort, 33285);

    using TSortTargetSelectors = int64_t(int64_t, int64_t);
    POINTER_SKYRIMSE(TSortTargetSelectors, sortTargetSelectors, 33282);

    arrayQuickSort(apArray, sortTargetSelectors, aUnk3, aUnk4);
}

void CombatController::UpdateTarget()
{
    for (auto* pTargetSelector : targetSelectors)
    {
        if ((pTargetSelector->flags & 2) == 0)
            pTargetSelector->Update();
    }

    if (targetSelectors.length > 1)
        ArrayQuickSortRecursiveCombatTargets(&targetSelectors, 0, targetSelectors.length);

    pActiveTargetSelector = nullptr;
    BSPointerHandle<Actor> newTarget{};

    if (targetSelectors.length)
    {
        for (auto* pTargetSelector : targetSelectors)
        {
            if ((pTargetSelector->flags & 1) != 0 && (pTargetSelector->flags & 2) == 0)
            {
                newTarget = pTargetSelector->SelectTarget();
                if (newTarget)
                {
                    pActiveTargetSelector = pTargetSelector;
                    break;
                }
            }
        }
    }

    if (newTarget.handle.iBits == targetHandle)
        return;

    Actor* pNewTarget = Cast<Actor>(TESObjectREFR::GetByHandle(newTarget.handle.iBits));
    SetTarget(pNewTarget);
}

TP_THIS_FUNCTION(TUpdateTarget, void, CombatController);
static TUpdateTarget* RealUpdateTarget = nullptr;

void TP_MAKE_THISCALL(HookUpdateTarget, CombatController)
{
    TiltedPhoques::ThisCall(RealUpdateTarget, apThis);

#if 0
    apThis->UpdateTarget();
#endif

#if 0
#if TP_SKYRIM64
    if (!apThis->startedCombat)
        return TiltedPhoques::ThisCall(RealUpdateTarget, apThis);
#endif

    return false;
#endif
}

TP_THIS_FUNCTION(TSetTarget, void, CombatController, Actor*);
static TSetTarget* RealSetTarget = nullptr;

void TP_MAKE_THISCALL(HookSetTarget, CombatController, Actor* apTarget)
{
#if TP_SKYRIM64
    spdlog::info("In game calls SetTarget, Form id: {:X}", apTarget->formID);
    TiltedPhoques::ThisCall(RealSetTarget, apThis, apTarget);
#endif
}

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
    #if TP_SKYRIM64
        POINTER_SKYRIMSE(TSetTarget, s_setTarget, 33235);
        POINTER_SKYRIMSE(TUpdateTarget, s_updateTarget, 33236);

        RealSetTarget = s_setTarget.Get();
        RealUpdateTarget = s_updateTarget.Get();

        TP_HOOK(&RealSetTarget, HookSetTarget);
        TP_HOOK(&RealUpdateTarget, HookUpdateTarget);
    #endif
    });

