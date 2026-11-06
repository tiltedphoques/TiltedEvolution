#pragma once

struct Actor;
struct MiddleProcess;
struct bhkCharacterController;

// Fallout 4 AIProcess: 0xE8. Middle-low, middle-high and high process data.
struct AIProcess
{
    bhkCharacterController* GetCharController() noexcept;
    void KnockExplosion(Actor* apActor, const NiPoint3* aSourceLocation, float afMagnitude) noexcept;
    void SetHeadTrackTarget(Actor* apOwner, const NiPoint3& acTarget) noexcept;

    // WEAPON_CULL_TYPE
    enum WeaponCullType : int32_t
    {
        kWeaponCullAnimation = 1,
        kWeaponCullWeaponSwitch = 2,
    };
    void SetWeaponBonesCulled(Actor* apActor, bool aCulled, WeaponCullType aType) noexcept;

    void* middleLowProcess;       // 0x00
    MiddleProcess* middleProcess; // 0x08 MiddleHighProcessData
    void* highProcess;            // 0x10
    uint8_t pad18[0xDF - 0x18];
    uint8_t processLevel;         // 0xDF
    uint8_t padE0[0xE8 - 0xE0];
};

static_assert(offsetof(AIProcess, middleProcess) == 0x8);
static_assert(offsetof(AIProcess, processLevel) == 0xDF);
static_assert(sizeof(AIProcess) == 0xE8);
