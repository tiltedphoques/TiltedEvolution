#pragma once

struct World;
struct TransportService;

struct NotifyObjectInventoryChanges;
struct NotifyInventoryChanges;
struct InventoryChangeEvent;
struct EquipmentChangeEvent;
struct NotifyEquipmentChanges;

struct InventoryService
{
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~InventoryService() noexcept = default;

    TP_NOCOPYMOVE(InventoryService);

    void OnInventoryChangeEvent(const InventoryChangeEvent& acEvent) noexcept;
    void OnEquipmentChangeEvent(const EquipmentChangeEvent& acEvent) noexcept;

    void OnNotifyInventoryChanges(const NotifyInventoryChanges& acMessage) noexcept;
    void OnNotifyEquipmentChanges(const NotifyEquipmentChanges& acMessage) noexcept;

private:

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    entt::scoped_connection m_inventoryConnection;
    entt::scoped_connection m_equipmentConnection;
    entt::scoped_connection m_inventoryChangeConnection;
    entt::scoped_connection m_equipmentChangeConnection;
};
