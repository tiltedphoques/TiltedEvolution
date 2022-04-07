#pragma once

struct World;
struct TransportService;

struct UpdateEvent;
struct NotifyObjectInventoryChanges;
struct NotifyInventoryChanges;
struct NotifyDrawWeapon;
struct InventoryChangeEvent;
struct EquipmentChangeEvent;
struct NotifyMagicEquipmentChanges;

struct InventoryService
{
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~InventoryService() noexcept = default;

    TP_NOCOPYMOVE(InventoryService);

    void OnUpdate(const UpdateEvent& acUpdateEvent) noexcept;
    void OnInventoryChangeEvent(const InventoryChangeEvent& acEvent) noexcept;
    void OnEquipmentChangeEvent(const EquipmentChangeEvent& acEvent) noexcept;

    void OnNotifyInventoryChanges(const NotifyInventoryChanges& acMessage) noexcept;
    void OnNotifyEquipmentChanges(const NotifyMagicEquipmentChanges& acMessage) noexcept;
    void OnNotifyDrawWeapon(const NotifyDrawWeapon& acMessage) noexcept;

private:

    void RunWeaponStateUpdates() noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_inventoryConnection;
    entt::scoped_connection m_equipmentConnection;
    entt::scoped_connection m_inventoryChangeConnection;
    entt::scoped_connection m_equipmentChangeConnection;
    entt::scoped_connection m_drawWeaponConnection;
};
