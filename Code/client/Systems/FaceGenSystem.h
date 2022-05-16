#pragma once

struct World;
struct Actor;
struct FaceGenComponent;
struct Tints;

/**
* @brief Manages the face gen of remote players.
*/
struct FaceGenSystem
{
    static void Update(World& aWorld, Actor* apActor, FaceGenComponent& aFaceGenComponent) noexcept;
    static void Setup(World& aWorld, entt::entity aEntity, const Tints& acTints) noexcept;
};
