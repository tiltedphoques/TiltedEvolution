#pragma once

struct PlayerLeaveEvent
{
    PlayerLeaveEvent(const entt::entity aEntity) : Entity{aEntity}
    {
    }

    entt::entity Entity;
};
