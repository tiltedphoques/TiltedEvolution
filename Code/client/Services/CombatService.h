#pragma once

struct World;
struct TransportService;
struct ProjectileLaunchedEvent;
struct NotifyProjectileLaunch;

/**
* @brief Responsible for projectiles, combat agro, etc.
*/
struct CombatService
{
public:
    CombatService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher);
    ~CombatService() noexcept = default;

    TP_NOCOPYMOVE(CombatService);

protected:
    void OnProjectileLaunchedEvent(const ProjectileLaunchedEvent& acEvent) const noexcept;
    void OnNotifyProjectileLaunch(const NotifyProjectileLaunch& acMessage) const noexcept;

private:
    World& m_world;
    TransportService& m_transport;

    entt::scoped_connection m_projectileLaunchedConnection;
    entt::scoped_connection m_projectileLaunchConnection;
};

