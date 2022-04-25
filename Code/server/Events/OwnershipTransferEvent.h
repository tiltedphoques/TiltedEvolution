#pragma once

/**
* Dispatched when a player gives up ownership of an actor,
* and when it is preferable for some other player client to claim ownership over it.
*/
struct OwnershipTransferEvent
{
    OwnershipTransferEvent(const entt::entity aEntity)
        : Entity{aEntity}
    {}

    entt::entity Entity;
};
