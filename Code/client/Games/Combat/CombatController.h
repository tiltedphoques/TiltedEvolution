#pragma once

struct CombatController
{
    void SetTarget(Actor* apTarget);

    uint8_t pad0[0x2C];
    uint32_t targetHandle;
    uint8_t pad30[0x5];
    bool startedCombat;
};

static_assert(offsetof(CombatController, targetHandle) == 0x2C);
static_assert(offsetof(CombatController, startedCombat) == 0x35);
