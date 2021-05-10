#pragma once

struct OwnershipTransferEvent
{
    OwnershipTransferEvent(const entt::entity aEntity)
        : Entity{aEntity}
    {}

    entt::entity Entity;
};
