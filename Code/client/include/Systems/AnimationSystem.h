#pragma once

#include <Components.h>

struct World;
struct Actor;

struct AnimationSystem
{
    static void Update(World& aWorld, Actor* apActor, RemoteAnimationComponent& aAnimationComponent, uint64_t aTick) noexcept;
    static void Setup(World& aWorld, entt::entity aEntity) noexcept;
    static void AddAction(RemoteAnimationComponent& aAnimationComponent, const TiltedMessages::ActionData& acActionData) noexcept;
};
