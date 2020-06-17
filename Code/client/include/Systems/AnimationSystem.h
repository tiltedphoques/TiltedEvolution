#pragma once

#include <Components.h>

struct World;
struct Actor;

struct AnimationSystem
{
    static void Update(World& aWorld, Actor* apActor, RemoteAnimationComponent& aAnimationComponent, uint64_t aTick) noexcept;
    static void Setup(World& aWorld, entt::entity aEntity) noexcept;
    static void AddAction(RemoteAnimationComponent& aAnimationComponent, const std::string& apData) noexcept;
    static void Serialize(World& aWorld, TiltedMessages::ReferenceMovementSnapshot& aMovementSnapshot, LocalComponent& localComponent, LocalAnimationComponent& animationComponent, FormIdComponent& formIdComponent);
    static bool Serialize(World& aWorld, const ActionEvent& aActionEvent, ActionEvent& aLastProcessedAction, std::string* apData);
};
