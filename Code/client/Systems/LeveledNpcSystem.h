#pragma once

struct TESNPC;

// Provides helpers for synchronizing leveled NPC identities.
struct LeveledNpcSystem
{
    // Returns whether a static NPC base still awaits a leveled-list pick.
    static bool IsUnresolvedLeveledShell(const TESNPC* apBase) noexcept;
};
