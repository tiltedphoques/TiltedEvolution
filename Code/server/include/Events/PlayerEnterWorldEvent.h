#pragma once

struct PlayerEnterWorldEvent
{
    PlayerEnterWorldEvent(const entt::entity aEntity)
        : Entity{aEntity}
    {}

    entt::entity Entity;
};
