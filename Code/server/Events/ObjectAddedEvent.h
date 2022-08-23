#pragma once

/**
 * @brief Dispatched whenever an object is added to the world.
 */
struct ObjectAddedEvent
{
    ObjectAddedEvent(const entt::entity aEntity) : Entity{aEntity}
    {
    }

    entt::entity Entity;
};
