#pragma once

struct World;
struct TransportService;

struct UpdateEvent;
struct NotifyObjectInventoryChanges;
struct NotifyCharacterInventoryChanges;
struct NotifyDrawWeapon;
struct InventoryChangeEvent;
struct EquipmentChangeEvent;

struct InventoryService
{
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~InventoryService() noexcept = default;

    TP_NOCOPYMOVE(InventoryService);

    void OnUpdate(const UpdateEvent& acUpdateEvent) noexcept;
    void OnInventoryChangeEvent(const InventoryChangeEvent& acEvent) noexcept;
    void OnEquipmentChangeEvent(const EquipmentChangeEvent& acEvent) noexcept;

    void OnObjectInventoryChanges(const NotifyObjectInventoryChanges& acMessage) noexcept;
    void OnCharacterInventoryChanges(const NotifyCharacterInventoryChanges& acMessage) noexcept;
    void OnNotifyDrawWeapon(const NotifyDrawWeapon& acMessage) noexcept;

private:

    void RunObjectInventoryUpdates() noexcept;
    void RunCharacterInventoryUpdates() noexcept;
    void RunWeaponStateUpdates() noexcept;

    void ApplyCachedObjectInventoryChanges() noexcept;
    void ApplyCachedCharacterInventoryChanges() noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    // TODO: these two can probably be merged now?
    Set<uint32_t> m_objectsWithInventoryChanges;
    Set<uint32_t> m_charactersWithInventoryChanges;
    Map<GameId, Inventory> m_cachedObjectInventoryChanges;
    Map<uint32_t, Inventory> m_cachedCharacterInventoryChanges;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_inventoryConnection;
    entt::scoped_connection m_equipmentConnection;
    entt::scoped_connection m_objectInventoryChangeConnection;
    entt::scoped_connection m_characterInventoryChangeConnection;
    entt::scoped_connection m_drawWeaponConnection;
};
