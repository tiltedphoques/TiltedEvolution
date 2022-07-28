#pragma once

struct World;
struct Actor;

/**
* @brief Used to cache data to compare against the current data.
* 
* This prevents an unnecessary amount of data usage.
*/
struct CacheSystem
{
    /**
    * @brief Sets up the cache for the relevant systems per actor.
    */
    static void Setup(World& aWorld, entt::entity aEntity, Actor* apActor) noexcept;
};
