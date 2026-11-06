#pragma once

struct World;
struct TransportService;

struct UpdateEvent;
struct NotifyObjectInventoryChanges;
struct NotifyInventoryChanges;
struct InventoryChangeEvent;
struct EquipmentChangeEvent;
struct NotifyEquipmentChanges;
struct DroppedItemPickupEvent;
struct NotifyDroppedItemPickup;
struct DisconnectedEvent;

/**
 * @brief Manages inventories of actors and containers.
 *
 * The initial contents of actor inventories are synced through spawn messages through CharacterService
 * @see CharacterService
 * The initial contents of container inventories are synced through the ObjectService
 * @see ObjectService
 * The InventoryService manages any changes to both the current equipment of actors,
 * and the contents of both actor and object inventories.
 */
struct InventoryService
{
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~InventoryService() noexcept = default;

    TP_NOCOPYMOVE(InventoryService);

    /**
     * @brief Check weapon state.
     * @see RunWeaponStateUpdates
     */
    void OnUpdate(const UpdateEvent& acUpdateEvent) noexcept;
    /**
     * @brief Sends out inventory changes made by the local client.
     */
    void OnInventoryChangeEvent(const InventoryChangeEvent& acEvent) noexcept;
    /**
     * @brief Sends out equipment changes made by the local client.
     */
    void OnEquipmentChangeEvent(const EquipmentChangeEvent& acEvent) noexcept;

    /**
     * @brief Applies inventory changes sent by the server.
     */
    void OnNotifyInventoryChanges(const NotifyInventoryChanges& acMessage) noexcept;
    /**
     * @brief Applies equipment changes sent by the server.
     */
    void OnNotifyEquipmentChanges(const NotifyEquipmentChanges& acMessage) noexcept;
    /**
     * @brief Tells other clients to remove their copy of a dropped item the local client picked up.
     */
    void OnDroppedItemPickupEvent(const DroppedItemPickupEvent& acEvent) noexcept;
    /**
     * @brief Removes the local copy of a dropped item another client picked up.
     */
    void OnNotifyDroppedItemPickup(const NotifyDroppedItemPickup& acMessage) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;

private:
    /**
     * Checks whether local actors their weapon draw states have changed,
     * and if so, send the new states to the server.
     */
    void RunWeaponStateUpdates() noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_inventoryConnection;
    entt::scoped_connection m_equipmentConnection;
    entt::scoped_connection m_inventoryChangeConnection;
    entt::scoped_connection m_equipmentChangeConnection;
    entt::scoped_connection m_droppedItemPickupConnection;
    entt::scoped_connection m_notifyDroppedItemPickupConnection;
    entt::scoped_connection m_disconnectedConnection;

    struct DroppedItem
    {
        uint32_t DropperId;
        uint32_t DropId;
        uint32_t BaseId;
    };
    // Dropped items in this client's world, keyed by the local reference form id
    TiltedPhoques::Map<uint32_t, DroppedItem> m_droppedItems;
    uint32_t m_nextDropId = 0;
};
