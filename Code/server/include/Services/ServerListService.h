#pragma once

struct World;
struct UpdateEvent;
struct PlayerJoinEvent;
struct PlayerLeaveEvent;

struct ServerListService
{
    ServerListService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~ServerListService() noexcept = default;

    TP_NOCOPYMOVE(ServerListService);

protected:

    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnPlayerJoin(const PlayerJoinEvent& acEvent) noexcept;
    void OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept;

private:

    void Announce() const noexcept;

    World& m_world;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_playerJoinConnection;
    entt::scoped_connection m_playerLeaveConnection;
    mutable std::chrono::steady_clock::time_point m_nextAnnounce;
};
