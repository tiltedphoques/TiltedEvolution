#pragma once

#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct World;
struct TransportService;

struct UpdateEvent;
struct NotifyObjectInventoryChanges;
struct NotifyCharacterInventoryChanges;
struct InventoryChangeEvent;

struct InventoryService
#if TP_SKYRIM64
    : public BSTEventSink<TESContainerChangedEvent>
#endif
{
    InventoryService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~InventoryService() noexcept = default;

    TP_NOCOPYMOVE(InventoryService);

    void OnUpdate(const UpdateEvent& acUpdateEvent) noexcept;
    void OnInventoryChangeEvent(const InventoryChangeEvent& acEvent) noexcept;

    void OnObjectInventoryChanges(const NotifyObjectInventoryChanges& acEvent) noexcept;
    void OnCharacterInventoryChanges(const NotifyCharacterInventoryChanges& acEvent) noexcept;

private:

#if TP_SKYRIM64
    BSTEventResult InventoryService::OnEvent(const TESContainerChangedEvent*, const EventDispatcher<TESContainerChangedEvent>*) override;
#endif

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
    entt::scoped_connection m_characterInventoryChangeConnection;
};
