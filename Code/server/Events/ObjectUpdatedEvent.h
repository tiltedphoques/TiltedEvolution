#pragma once

/**
* @brief Dispatched whenever an object received an update.
*/
struct ObjectUpdatedEvent
{
    ObjectUpdatedEvent(const entt::entity aEntity)
        : Entity{aEntity}
    {}

    entt::entity Entity;
};
