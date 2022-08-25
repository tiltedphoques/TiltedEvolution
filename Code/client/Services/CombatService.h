#pragma once

struct World;
struct TransportService;
struct UpdateEvent;
struct ProjectileLaunchedEvent;
struct NotifyProjectileLaunch;
struct HitEvent;

/**
* @brief Responsible for projectiles, combat agro, etc.
*/
struct CombatService
{
    CombatService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher);
    ~CombatService() noexcept = default;

    TP_NOCOPYMOVE(CombatService);

protected:
    void OnUpdate(const UpdateEvent& acEvent) const noexcept;
    void OnLocalComponentRemoved(entt::registry& aRegistry, entt::entity aEntity) const noexcept;
    void OnProjectileLaunchedEvent(const ProjectileLaunchedEvent& acEvent) const noexcept;
    void OnNotifyProjectileLaunch(const NotifyProjectileLaunch& acMessage) const noexcept;
    void OnHitEvent(const HitEvent& acEvent) const noexcept;

    void RunTargetUpdates(const float acDelta) const noexcept;

private:
    World& m_world;
    TransportService& m_transport;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_localComponentRemoved;
    entt::scoped_connection m_projectileLaunchedConnection;
    entt::scoped_connection m_projectileLaunchConnection;
    entt::scoped_connection m_hitConnection;
};

