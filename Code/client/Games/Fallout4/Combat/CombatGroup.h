#pragma once

#include <AI/AITimer.h>
#include <Games/Misc/BGSWorldLocation.h>

struct CombatTarget
{
    uint32_t targetHandle;
    float weightedDPS;
    float combatStrength;
    uint32_t flags;
    int32_t detectLevel;
    float stealthPoints;
    BGSWorldLocation lastKnownLoc;
    BGSWorldLocation lastDetectedLoc;
    BGSWorldLocation lastSeenLoc;
    BGSWorldLocation lastVisibleLoc;
    BGSWorldLocation lastAttackLoc;
    BGSWorldLocation lastDetectedEventLoc;
    float lastSeenTimeStamp;
    float lastDetectedTimeStamp;
    float lastKnownTimeStamp;
    float lastAttackTimeStamp;
    float lastDetectedEventTimeStamp;
    float suspectedTimeStamp;
    float lostTimeStamp;
    uint32_t detectingMember;
    uint16_t attackerCount;
    uint8_t padCA[6];
};

struct CombatMember
{
    BSPointerHandle<Actor> hMember;
    float weightedDPS;
    float combatStrength;
    uint32_t flags;
    void* cluster;
};

struct CombatSearchLocation
{
    BGSWorldLocation loc;
    BSPointerHandle<Actor> actor;
    uint32_t type;
    float timestamp;
    float radius;
    uint32_t flags;
    uint32_t pad2C;
};

struct CombatGroup
{
    uint32_t groupID;
    uint32_t groupIndex;
    GameArray<CombatTarget> targets;
    GameArray<CombatMember> members;
    void* detectionListener;
    GameArray<void*> clusters;
    AITimer combatStrengthTimer;
    AITimer updateTimer;
    AITimer musicUpdateTimer;
    AITimer stealthPointUpdateTimer;
    void* combatBlackboard;
    float memberTotalHealth;
    float memberWeightedDPS;
    float memberCombatStrength;
    float targetTotalHealth;
    float targetWeightedDPS;
    float targetCombatStrength;
    uint32_t searchCount;
    uint32_t pad9C;
    void* gridMap;
    AITimer searchUpdateTimer;
    AITimer searchAreaUpdateTimer;
    float searchStartedTimeStamp;
    BSPointerHandle<Actor> searchingMember;
    BGSWorldLocation searchCenter;
    float searchRadius;
    uint32_t padDC;
    GameArray<CombatSearchLocation> searchLocations;
    uint8_t searchTeleportDoors[0x18];
    uint32_t initializedMemberCount;
    uint32_t fleeCount;
    uint32_t fightCount;
    uint8_t musicState;
    uint8_t pad11D[3];
    BSReadWriteLock lock;
};

static_assert(offsetof(CombatTarget, detectLevel) == 0x10);
static_assert(offsetof(CombatTarget, attackerCount) == 0xC8);
static_assert(sizeof(CombatTarget) == 0xD0);
static_assert(sizeof(CombatMember) == 0x18);
static_assert(sizeof(CombatSearchLocation) == 0x30);
static_assert(offsetof(CombatGroup, targets) == 0x8);
static_assert(offsetof(CombatGroup, searchLocations) == 0xE0);
static_assert(offsetof(CombatGroup, lock) == 0x120);
static_assert(sizeof(CombatGroup) == 0x128);
