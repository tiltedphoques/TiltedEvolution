#pragma once

#include <AI/AITimer.h>

struct CombatController;

struct CombatTargetSelector
{
    uint8_t combatObject[0x10];
    CombatController* pCombatController;
    BSPointerHandle<Actor> hTarget;
    uint32_t ePriority;
    uint32_t flags;
    uint32_t pad24;
};

struct CombatTargetSelectorStandard : CombatTargetSelector
{
    AITimer updateTimer;
};

static_assert(offsetof(CombatTargetSelector, pCombatController) == 0x10);
static_assert(offsetof(CombatTargetSelector, ePriority) == 0x1C);
static_assert(sizeof(CombatTargetSelector) == 0x28);
static_assert(sizeof(CombatTargetSelectorStandard) == 0x30);
