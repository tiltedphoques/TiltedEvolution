#pragma once

struct CombatTargetSelector;

struct CombatController
{
    void SetTarget(Actor* apTarget);
    void UpdateTarget();

    uint8_t pad0[0x2C];
    uint32_t targetHandle;
    uint8_t pad30[0x5];
    bool startedCombat;
    uint8_t unk36[0x62];
    GameArray<CombatTargetSelector*> targetSelectors;
    uint8_t unkB0[8];
    CombatTargetSelector* pActiveTargetSelector;
};

static_assert(offsetof(CombatController, targetHandle) == 0x2C);
static_assert(offsetof(CombatController, startedCombat) == 0x35);
static_assert(offsetof(CombatController, targetSelectors) == 0x98);
static_assert(offsetof(CombatController, pActiveTargetSelector) == 0xB8);
