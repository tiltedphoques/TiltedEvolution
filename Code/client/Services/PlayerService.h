#pragma once

struct World;
struct TransportService;

struct UpdateEvent;
struct DisconnectedEvent;
struct ServerSettings;
struct GridCellChangeEvent;
struct CellChangeEvent;
struct PlayerDialogueEvent;
struct PlayerMapMarkerUpdateEvent;
struct PlayerLevelEvent;
struct PlayerLevelEvent;
struct PlayerMapMarkerUpdateEvent;
struct NotifyPlayerRespawn;
struct NotifyPlayerPosition;
struct NotifyPlayerJoined;
struct NotifyPlayerLeft;
struct NotifyPlayerCellChanged;
struct GridCellCoords;

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
    void OnDisconnected(const DisconnectedEvent& acEvent) noexcept;
    void OnServerSettingsReceived(const ServerSettings& acSettings) noexcept;
    void OnPlayerJoined(const NotifyPlayerJoined& acMessage) noexcept;
    void OnPlayerLeft(const NotifyPlayerLeft& acMessage) noexcept;
    void OnNotifyPlayerRespawn(const NotifyPlayerRespawn& acMessage) const noexcept;
    void OnGridCellChangeEvent(const GridCellChangeEvent& acEvent) const noexcept;
    void OnCellChangeEvent(const CellChangeEvent& acEvent) const noexcept;
    void OnPlayerDialogueEvent(const PlayerDialogueEvent& acEvent) const noexcept;
    void OnPlayerMapMarkerUpdateEvent(const PlayerMapMarkerUpdateEvent& acEvent) const noexcept;
    void OnPlayerLevelEvent(const PlayerLevelEvent& acEvent) const noexcept;
    void OnNotifyPlayerPosition(const NotifyPlayerPosition& acMessage) const noexcept;
    void OnNotifyPlayerCellChanged(const NotifyPlayerCellChanged& acMessage) const noexcept;

private:

    TESObjectCELL* GetCell(const GameId& acCellId, const GameId& acWorldSpaceId, const GridCellCoords& acCenterCoords) const noexcept;

    /**
    * @brief Run the respawn timer, and if it hits 0, respawn the player.
    */
    void RunRespawnUpdates(const double acDeltaTime) noexcept;
    void RunPostDeathUpdates(const double acDeltaTime) noexcept;
    /**
    * @brief Make sure difficulty doesn't get changed while connected
    */
    void RunDifficultyUpdates() const noexcept;
    void RunLevelUpdates() const noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    double m_respawnTimer = 0.0;
    int32_t m_serverDifficulty = 6;
    int32_t m_previousDifficulty = 6;

    struct MapInfo
    {
        void Delete() const noexcept;

        // TODO: these raw ptrs are a bad idea
        TESObjectREFR* pPlayer;
        MapMarkerData* pMarkerData;
    };

    TiltedPhoques::Map<uint32_t, MapInfo> m_mapHandles;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_settingsConnection;
    entt::scoped_connection m_playerJoinedConnection;
    entt::scoped_connection m_playerLeftConnection;
    entt::scoped_connection m_notifyRespawnConnection;
    entt::scoped_connection m_gridCellChangeConnection;
    entt::scoped_connection m_cellChangeConnection;
    entt::scoped_connection m_playerDialogueConnection;
    entt::scoped_connection m_playerMapMarkerConnection;
    entt::scoped_connection m_playerLevelConnection;
    entt::scoped_connection m_playerPositionConnection;
    entt::scoped_connection m_playerCellChangeConnection;
};
