#pragma once

#include <AI/AITimer.h>

struct CombatGroup;
struct CombatState;
struct CombatInventory;
struct CombatAimController;
struct CombatTargetSelector;

struct CombatController
{
    void SetTarget(Actor* apTarget);
    void UpdateTarget();

    uint8_t eventSource[0x40];
    CombatGroup* pCombatGroup;
    CombatState* pState;
    CombatInventory* pInventory;
    void* pCombatBlackboard;
    void* pBehaviorController;
    uint32_t attackerHandle;
    uint32_t targetHandle;
    uint32_t previousTargetHandle;
    uint32_t currentStance;
    uint32_t desiredStance;
    uint32_t pad7C;
    TESCombatStyle* pCombatStyle;
    AITimer updateTimer;
    float lowMovementDelta;
    uint32_t pad94;
    GameArray<void*> aimControllers;
    CombatAimController* pActiveAimController;
    CombatAimController* pCurrentAimController;
    GameArray<void*> viewControllers;
    void* pActiveViewController;
    GameArray<void*> areas;
    CombatAreaStandard* pCurrentArea;
    GameArray<CombatTargetSelector*> targetSelectors;
    CombatTargetSelector* pDefaultTargetSelector;
    CombatTargetSelector* pActiveTargetSelector;
    GameArray<void*> combatObjects;
    uint8_t behaviorTreeMap[0x30];
    uint32_t handleCount;
    uint32_t pad174;
    NiPointer<Actor> pCachedAttacker;
    NiPointer<Actor> pCachedTarget;
    bool firstUpdate;
    bool stoppedCombat;
    bool stoppingCombat;
    bool paused;
    bool inactive;
    uint8_t pad18D[3];
};

static_assert(offsetof(CombatController, pCombatGroup) == 0x40);
static_assert(offsetof(CombatController, targetHandle) == 0x6C);
static_assert(offsetof(CombatController, targetSelectors) == 0x100);
static_assert(offsetof(CombatController, pActiveTargetSelector) == 0x120);
static_assert(offsetof(CombatController, pCachedAttacker) == 0x178);
static_assert(sizeof(CombatController) == 0x190);
