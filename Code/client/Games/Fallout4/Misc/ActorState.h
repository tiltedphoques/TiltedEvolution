#pragma once

#include <Misc/IMovementState.h>

// Fallout 4 ActorState: IMovementState base (0x8) + two flag words.
// Bit layout from libxse/commonlibf4 Actor.h.
struct ActorState : IMovementState
{
    virtual ~ActorState();

    virtual bool SetWeaponMagicDrawn(bool aDrawn);
    virtual bool SetWeaponState(uint32_t aState);
    virtual bool DoSetSitSleepState(uint32_t aState);
    virtual uint32_t DoGetSitSleepState() const;
    virtual bool SetInIronSightsImpl(bool aSighted);
    virtual void SetReloadingImpl(bool aReloading);

    uint32_t flags1; // 0x08: moveMode, flyState, lifeState, knockState,
                     // meleeAttackState, talkingToPlayer, forceRun,
                     // forceSneak, headTracking
    uint32_t flags2; // 0x0C: reanimating, weaponState, wantBlocking,
                     // flightBlocked, recoil, allowFlying, staggered,
                     // inWrongProcessLevel, stance, gunState,
                     // interactingState, headTrackRotation, inSyncAnim

    bool IsWeaponDrawn() const noexcept { return (flags2 >> 1 & 7) >= 3; }

    bool IsWeaponFullyDrawn() const noexcept { return (flags2 >> 1 & 7) == 3; }

    // FO4 lifeState lives in flags1[17:20]; the two "dead" encodings the
    // client cares about map to the same pattern as Skyrim's.
    bool IsBleedingOut() const noexcept { return (flags1 & 0x1E00000) == 0x1000000 || (flags1 & 0x1E00000) == 0xE00000; }

    bool SetWeaponDrawn(bool aDraw) noexcept;
};

static_assert(sizeof(ActorState) == 0x10);
