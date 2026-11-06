#pragma once

#include <optional>

struct World;

/**
 * @brief Ownership epoch rules for actors, shared by the services that send and apply owner updates.
 *
 * The server only accepts and relays updates from an actor's current owner at its current epoch.
 */
struct OwnershipSystem
{
    struct Ownership
    {
        uint32_t ServerId{};
        uint32_t OwnershipEpoch{};
    };

    /**
     * @brief Returns the remote entity a relayed owner update applies to, or nothing if its epoch is stale.
     *
     * A newer epoch means this client missed the ownership transfer, so it is adopted instead of dropping every later update.
     */
    static std::optional<entt::entity> AcceptRemoteUpdate(World& aWorld, uint32_t aServerId, uint32_t aOwnershipEpoch) noexcept;
    /**
     * @brief Returns the local entity a relayed update for one of our own actors applies to, e.g. another player looting it.
     */
    static std::optional<entt::entity> AcceptOwnerUpdate(World& aWorld, uint32_t aServerId, uint32_t aOwnershipEpoch) noexcept;
    /**
     * @brief Returns the server id and epoch to send a local change to an actor with, or nothing if it isn't ours to send.
     *
     * @param aAllowRemote Also allows actors owned by someone else, for changes the local player makes to them (looting, pickpocketing).
     */
    static std::optional<Ownership> GetOutgoingOwnership(World& aWorld, entt::entity aEntity, bool aAllowRemote) noexcept;
    /**
     * @brief Returns the ownership epoch this client knows for the actor, or 0 if it is neither local nor remote.
     */
    static uint32_t GetOwnershipEpoch(World& aWorld, entt::entity aEntity) noexcept;
};
