#pragma once

struct CombatInventory
{
    uint8_t pad0[0x1B0];
    float maximumRange;
    float optimalMinRange;
    float optimalMaxRange;
    float actorExtents;
    float actorCollisionExtents;
    float effectiveDistanceMin;
    float effectiveDistanceMax;
    float maximumOffensiveRange;
    float maximumOptimalOffensiveRange;
    float minimumRange;
    float optimalRangeMult;
    float detectionRange;
    uint32_t itemRestrictions;
    uint16_t availableEquipSlots;
    uint16_t defaultEquipmentSlots;
    uint32_t flags;
    BSReadWriteLock lock;
    uint32_t pad1F4;
};

static_assert(offsetof(CombatInventory, maximumRange) == 0x1B0);
static_assert(sizeof(CombatInventory) == 0x1F8);
