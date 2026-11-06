#pragma once

#include <Events/PacketEvent.h>
#include <Components.h>
#include <Structs/Inventory.h>
#include <Structs/Vector3_NetQuantize.h>

struct World;
struct UpdateEvent;
struct RequestObjectInventoryChanges;
struct RequestInventoryChanges;
struct RequestEquipmentChanges;
struct DrawWeaponRequest;
struct RequestDroppedItemPickup;
struct NotifyDroppedItem;
struct EnterExteriorCellRequest;
struct EnterInteriorCellRequest;
struct ShiftGridCellRequest;
struct Player;
struct PlayerLeaveCellEvent;

/**
 * @brief Relays inventory/equipment changes and updates the server side state.
 */
class InventoryService
{
public:
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher);

    /**
     * @brief Relays inventory changes to other clients and updates server side inventories.
     */
    void OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) noexcept;
    /**
     * @brief Relays equipment changes to other clients and updates server side equipment.
     */
    void OnEquipmentChanges(const PacketEvent<RequestEquipmentChanges>& acMessage) noexcept;
    /**
     * @brief Relays weapon draw changes to other clients and updates server side weapon draw state.
     */
    void OnWeaponDrawnRequest(const PacketEvent<DrawWeaponRequest>& acMessage) noexcept;
    /**
     * @brief Relays that a dropped item was picked up, so other clients remove their copy.
     */
    void OnDroppedItemPickup(const PacketEvent<RequestDroppedItemPickup>& acMessage) noexcept;
    /**
     * @brief Sends the items lying in the cell a player enters.
     */
    void OnExteriorCellEnter(const PacketEvent<EnterExteriorCellRequest>& acMessage) const noexcept;
    void OnInteriorCellEnter(const PacketEvent<EnterInteriorCellRequest>& acMessage) const noexcept;
    void OnGridCellShift(const PacketEvent<ShiftGridCellRequest>& acMessage) const noexcept;

private:
    // An item a player dropped that nobody has picked up yet
    struct DroppedItem
    {
        uint32_t DropperId{};
        uint32_t DropId{};
        Inventory::Entry Item{};
        Vector3_NetQuantize Position{};
        CellIdComponent Cell{};
    };

    void AddDroppedItem(entt::entity aDropper, const RequestInventoryChanges& acMessage, const Player* apSender) noexcept;
    void SendDroppedItems(Player* apPlayer, const CellIdComponent& acCell) const noexcept;
    static void Serialize(const DroppedItem& acItem, NotifyDroppedItem& aNotify) noexcept;

    World& m_world;

    entt::scoped_connection m_inventoryChangeConnection;
    entt::scoped_connection m_equipmentChangeConnection;
    entt::scoped_connection m_drawWeaponConnection;
    entt::scoped_connection m_droppedItemPickupConnection;
    entt::scoped_connection m_exteriorCellEnterConnection;
    entt::scoped_connection m_interiorCellEnterConnection;
    entt::scoped_connection m_gridCellShiftConnection;

    Vector<DroppedItem> m_droppedItems;
};
