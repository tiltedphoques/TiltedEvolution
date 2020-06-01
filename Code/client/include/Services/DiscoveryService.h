#pragma once

struct World;

struct ConnectedEvent;
struct GameService;

struct PreUpdateEvent;
struct ReferenceSpawnedEvent;

struct TESObjectREFR;

struct DiscoveryService final
{
    DiscoveryService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~DiscoveryService() noexcept = default;

    TP_NOCOPYMOVE(DiscoveryService);

protected:

    void VisitCell(bool aForceTrigger = false) noexcept;
    void VisitForms() noexcept;

    void OnUpdate(const PreUpdateEvent& acPreUpdateEvent) noexcept;

    void OnConnected(const ConnectedEvent& acEvent) noexcept;

private:

    World& m_world;
    entt::dispatcher& m_dispatcher;

    Set<uint32_t> m_forms;
    uint32_t m_cellId = 0;

    entt::scoped_connection m_preUpdateConnection;
    entt::scoped_connection m_connectedConnection;
};
