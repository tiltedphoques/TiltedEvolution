#pragma once

struct World;
struct UpdateEvent;
struct PlayerJoinEvent;
struct PlayerLeaveEvent;

/**
* @brief Dispatches the current player list to the clients.
*/
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

    static void PostAnnouncement(
        String acName, 
        String acDesc, 
        String acIconUrl, 
        uint16_t aPort, 
        uint16_t aTick, 
        uint16_t aPlayerCount, 
        uint16_t aPlayerMaxCount, 
        String acTagList,
        bool aPublic) noexcept;

    World& m_world;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_playerJoinConnection;
    entt::scoped_connection m_playerLeaveConnection;
    mutable std::chrono::steady_clock::time_point m_nextAnnounce;
};
