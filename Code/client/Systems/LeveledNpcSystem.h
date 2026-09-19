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

    // Rebuilds the actor's base from its original NPC and the selected template,
    // updates leveled-creature metadata, and disposes of the old temporary base.
    // Requires an actor that has finished disabling and has no 3D.
    // Returns false if the base cannot be rebuilt, leaving the actor unchanged.
    static bool ApplyPick(Actor* apActor, TESNPC* apPick) noexcept;
};
