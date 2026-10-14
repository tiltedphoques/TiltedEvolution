#pragma once

#include <Events/PacketEvent.h>

struct World;
struct Player;
struct CellIdComponent;
struct DropItemRequest;
struct PickupDroppedItemRequest;
struct EnterInteriorCellRequest;
struct EnterExteriorCellRequest;
struct ShiftGridCellRequest;

/**
 * @brief Tracks items players drop into the world.
 *
 * Every dropped item gets its own entity, so that every client places the same item,
 * players entering the area later on see it too, and only one player can pick it up.
 */
class DroppedItemService
{
public:
    DroppedItemService(World& aWorld, entt::dispatcher& aDispatcher);

private:
    void OnDropItemRequest(const PacketEvent<DropItemRequest>& acMessage) noexcept;
    void OnPickupDroppedItemRequest(const PacketEvent<PickupDroppedItemRequest>& acMessage) noexcept;
    void OnInteriorCellEnter(const PacketEvent<EnterInteriorCellRequest>& acMessage) const noexcept;
    void OnExteriorCellEnter(const PacketEvent<EnterExteriorCellRequest>& acMessage) const noexcept;
    void OnGridCellShift(const PacketEvent<ShiftGridCellRequest>& acMessage) const noexcept;

    /**
     * @brief Sends every dropped item in range of the given cell to the player.
     */
    void SendDroppedItemsInRange(Player* apPlayer, const CellIdComponent& acCell) const noexcept;
    /**
     * @brief Removes a dropped item and tells everyone, except for the given player, to remove it.
     */
    void RemoveDroppedItem(entt::entity aEntity, const Player* apExcludedPlayer) noexcept;
    void EnforceDroppedItemLimit() noexcept;

    World& m_world;
    uint64_t m_nextDropOrder{0};

    entt::scoped_connection m_dropItemConnection;
    entt::scoped_connection m_pickupDroppedItemConnection;
    entt::scoped_connection m_interiorCellEnterConnection;
    entt::scoped_connection m_exteriorCellEnterConnection;
    entt::scoped_connection m_gridCellShiftConnection;
};
