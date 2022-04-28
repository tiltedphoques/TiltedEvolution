#pragma once

struct World;
struct TransportService;

struct UpdateEvent;

/**
* @brief Handles logic related to the local player.
*/
struct PlayerService
{
    PlayerService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~PlayerService() noexcept = default;

    TP_NOCOPYMOVE(PlayerService);

protected:

    void OnUpdate(const UpdateEvent& acEvent) noexcept;

private:

    /**
    * @brief Run the respawn timer, and if it hits 0, respawn the player.
    */
    void RunRespawnUpdates(const double acDeltaTime) noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    double m_respawnTimer = 0.0;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_deathConnection;
};
