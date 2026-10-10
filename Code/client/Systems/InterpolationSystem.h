#pragma once

struct World;
struct Actor;

/**
 * @brief Manages interpolation of movement and animations.
 */
struct InterpolationSystem
{
    // aNow and aArrivalTime are on the same local clock, see PlaybackClock
    static void Update(Actor* apActor, InterpolationComponent& aInterpolationComponent, double aNow) noexcept;
    static void AddPoint(InterpolationComponent& aInterpolationComponent, InterpolationComponent::TimePoint aPoint, double aArrivalTime) noexcept;
    // The owner tick that remote actions should be played up to, 0 until the first snapshot has arrived
    static uint64_t GetPlaybackTick(const InterpolationComponent& acInterpolationComponent) noexcept;
    static InterpolationComponent& Setup(World& aWorld, entt::entity aEntity) noexcept;
    static void Clean(World& aWorld, entt::entity aEntity) noexcept;
};
