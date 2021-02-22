#pragma once

struct World;
struct Actor;

struct CacheSystem
{
    static void Setup(World& aWorld, entt::entity aEntity, Actor* apActor) noexcept;
};
