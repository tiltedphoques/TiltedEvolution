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

    // If CombatComponent is attached, don't try to fetch a new target.
    if (Actor* pAttacker = Cast<Actor>(TESObjectREFR::GetByHandle(attackerHandle)))
    {
        const auto view = World::Get().view<FormIdComponent, CombatComponent>();
        const auto it = std::find_if(view.begin(), view.end(), [view, pAttacker](auto entity) { return view.get<FormIdComponent>(entity).Id == pAttacker->formID; });
        if (it != view.end())
            return;
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
    apThis->UpdateTarget();
}

void CombatController::SetTarget(Actor* apTarget)
{
#if TP_SKYRIM64
    TP_THIS_FUNCTION(TSetTarget, void, CombatController, Actor*);
    POINTER_SKYRIMSE(TSetTarget, setTarget, 33235);
    TiltedPhoques::ThisCall(setTarget, this, apTarget);
#endif
}

static TiltedPhoques::Initializer s_combatControllerHooks(
    []()
    {
    #if TP_SKYRIM64
        POINTER_SKYRIMSE(TUpdateTarget, s_updateTarget, 33236);

        RealUpdateTarget = s_updateTarget.Get();

        TP_HOOK(&RealUpdateTarget, HookUpdateTarget);
    #endif
    });

