#pragma once

struct TESNPC;
struct Actor;

// Provides helpers for synchronizing leveled NPC identities.
struct LeveledNpcSystem
{
    // Returns whether a non-temporary NPC base uses a leveled-character template.
    static bool IsLeveledNpcBase(const TESNPC* apBase) noexcept;

    // The placed NPC owns the template flags and all non-inherited data.
    // A resolved actor retains it in ExtraLeveledCreature::originalBase.
    static TESNPC* GetOriginalBase(const Actor* apActor) noexcept;
};
