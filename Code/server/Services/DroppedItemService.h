#pragma once

#include <Events/PacketEvent.h>

struct World;
struct UpdateEvent;
struct PlayerLeaveEvent;
struct DropItemRequest;
struct DroppedItemMoveRequest;
struct PickUpDroppedItemRequest;
struct DroppedItemComponent;
struct DroppedItemData;
struct ServerMessage;
struct Player;

/**
 * @brief Registers dropped items for the whole session (until picked up or evicted) and relays them to players in range.
 * The server tells each client when to spawn and delete its copies, so no untracked copy can be picked up twice.
 */
class DroppedItemService
{
public:
    DroppedItemService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~DroppedItemService() noexcept = default;

    TP_NOCOPYMOVE(DroppedItemService);

private:
    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept;
    void OnDropItem(const PacketEvent<DropItemRequest>& acMessage) noexcept;
    void OnDroppedItemMove(const PacketEvent<DroppedItemMoveRequest>& acMessage) noexcept;
    void OnPickUpDroppedItem(const PacketEvent<PickUpDroppedItemRequest>& acMessage) noexcept;

    /**
     * Spawns items that came into each player's range and removes the ones that left it.
     */
    void UpdateInterest() noexcept;
    void EvictOldestItems() noexcept;
    void RemoveItem(entt::entity aEntity, const Player* apExcludedPlayer) noexcept;
    /**
     * Marks an item at rest when its simulating client can no longer stream it.
     */
    void StopSimulation(entt::entity aEntity, DroppedItemComponent& aDroppedItem) noexcept;
    void SendToPlayersWithItem(const ServerMessage& acMessage, entt::entity aEntity, const Player* apExcludedPlayer) const noexcept;
    DroppedItemData ToData(entt::entity aEntity) const noexcept;

    World& m_world;

    // Items each player has been told to spawn.
    TiltedPhoques::Map<Player*, TiltedPhoques::Set<entt::entity>> m_knownItems{};
    uint64_t m_nextSequence{};
    float m_timeSinceInterestUpdate{};

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_playerLeaveConnection;
    entt::scoped_connection m_dropItemConnection;
    entt::scoped_connection m_droppedItemMoveConnection;
    entt::scoped_connection m_pickUpDroppedItemConnection;
};
