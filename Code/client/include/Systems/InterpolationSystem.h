#pragma once

#include <Components.h>

struct World;
struct Actor;

struct InterpolationSystem
{
    static void Update(Actor* apActor, InterpolationComponent& aInterpolationComponent, uint64_t aTick) noexcept;
    static void AddPoint(InterpolationComponent& aInterpolationComponent, const InterpolationComponent::TimePoint& acPoint ) noexcept;
    static void Setup(World& aWorld, entt::entity aEntity) noexcept;
};
