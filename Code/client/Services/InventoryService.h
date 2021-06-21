#pragma once

struct World;
struct TransportService;

struct UpdateEvent;
struct NotifyObjectInventoryChanges;
struct InventoryChangeEvent;

struct InventoryService
{
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~InventoryService() noexcept = default;

    TP_NOCOPYMOVE(InventoryService);

    void OnUpdate(const UpdateEvent& acUpdateEvent) noexcept;
    void OnInventoryChangeEvent(const InventoryChangeEvent& acEvent) noexcept;
    void OnObjectInventoryChanges(const NotifyObjectInventoryChanges& acEvent) noexcept;

private:
    void RunObjectInventoryUpdates() noexcept;
    void RunCharacterInventoryUpdates() noexcept;

    void ApplyCachedObjectInventoryChanges() noexcept;
    void ApplyCachedCharacterInventoryChanges() noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    Set<uint32_t> m_objectsWithInventoryChanges;
    Set<uint32_t> m_charactersWithInventoryChanges;
    Map<GameId, Inventory> m_cachedObjectInventoryChanges;
    Map<uint32_t, Inventory> m_cachedCharacterInventoryChanges;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_inventoryConnection;
    entt::scoped_connection m_objectInventoryChangeConnection;
};
