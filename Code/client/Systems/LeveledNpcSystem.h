#pragma once

struct World;
struct Actor;
struct TESNPC;
struct GameId;

struct LeveledNpcSystem
{
    // Connects conform component lifecycle signals to the actor's re-enabling flag.
    static void Initialize(entt::registry& aRegistry) noexcept;
    // Resolves the canonical spawn base, falling back to the placed base if needed.
    static TESNPC* ResolveCustomSpawnNpcBase(World& aWorld, const GameId& acPickId, const GameId& acBaseId) noexcept;
    // Checks whether a static NPC base still points to a leveled-character template.
    static bool IsUnresolvedShell(const TESNPC* apBase) noexcept;

private:
    // Pauses animation and appearance sync when reconstruction starts.
    static void OnConformStarted(entt::registry& aRegistry, entt::entity aEntity) noexcept;
    // Invalidates the animation descriptor and resumes sync when the marker is removed.
    static void OnConformFinished(entt::registry& aRegistry, entt::entity aEntity) noexcept;
};
