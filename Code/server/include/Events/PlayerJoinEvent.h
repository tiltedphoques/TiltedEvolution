#pragma once

struct PlayerJoinEvent
{
    PlayerJoinEvent(const entt::entity aEntity) : Entity{aEntity}
    {
    }

    entt::entity Entity;
};
