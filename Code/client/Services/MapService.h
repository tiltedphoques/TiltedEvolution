#pragma once

struct World;
struct TransportService;

struct UpdateEvent;
struct DisconnectedEvent;
struct CellChangeEvent;
struct MenuOpenEvent;
struct MenuCloseEvent;
struct PlayerMapMarkerUpdateEvent;
struct PlayerMapMarkerUpdateEvent;
struct SetWaypointEvent;
struct DeleteWaypointEvent;
struct NotifySetWaypoint;
struct NotifyDeleteWaypoint;
struct NotifyPlayerPosition;
struct NotifyPlayerJoined;
struct NotifyPlayerLeft;
struct NotifyPlayerCellChanged;
struct GridCellCoords;

/**
 * @brief Handles logic related to the local player.
 */
struct MapService
{
    MapService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~MapService() noexcept = default;

    TP_NOCOPYMOVE(MapService);

  protected:
    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnConnected(const ConnectedEvent& acEvent) noexcept;
    void OnDisconnected(const DisconnectedEvent& acEvent) noexcept;
    void OnPlayerJoined(const NotifyPlayerJoined& acMessage) noexcept;
    void OnPlayerLeft(const NotifyPlayerLeft& acMessage) noexcept;
    void OnNotifyPlayerSetWaypoint(const NotifySetWaypoint& acMessage) noexcept;
    void OnNotifyPlayerDelWaypoint(const NotifyDeleteWaypoint& acMessage) noexcept;
    void OnNotifyPlayerPosition(const NotifyPlayerPosition& acMessage) const noexcept;
    void OnNotifyPlayerCellChanged(const NotifyPlayerCellChanged& acMessage) const noexcept;
    void OnPlayerSetWaypoint(const SetWaypointEvent& acMessage) noexcept;
    void OnPlayerDelWaypoint(const DeleteWaypointEvent& acMessage) noexcept;
    void OnMenuClose(const MenuCloseEvent& acMessage) noexcept;

  private:
    TESObjectCELL* GetCell(const GameId& acCellId, const GameId& acWorldSpaceId,
                           const GridCellCoords& acCenterCoords) const noexcept;

    void RunMapUpdates() noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    void CreateDummyMarker() noexcept;
    uint32_t GetDummyMarker() noexcept;
    bool DeleteDummyMarker(const uint32_t acHandle) noexcept;

    NiPoint3 m_waypointPos;
    TESObjectREFR* m_waypoint;
    MapMarkerData* m_waypointData;
    bool m_inMap;
    bool m_waypointActive;
    int m_invalidHandle = -1;
    int m_initDummyMarkers = 10;

    int32_t m_serverDifficulty = 6;
    int32_t m_previousDifficulty = 6;

    Vector<uint32_t> m_dummyHandles;
    TiltedPhoques::Map<uint32_t, uint32_t> m_mapHandles;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_playerJoinedConnection;
    entt::scoped_connection m_playerLeftConnection;
    entt::scoped_connection m_cellChangeConnection;;
    entt::scoped_connection m_playerMapMarkerConnection;
    entt::scoped_connection m_playerPositionConnection;
    entt::scoped_connection m_playerCellChangeConnection;
    entt::scoped_connection m_playerDelWaypointConnection;
    entt::scoped_connection m_playerSetWaypointConnection;
    entt::scoped_connection m_playerNotifySetWaypointConnection;
    entt::scoped_connection m_playerNotifyDeleteWaypointConnection;
    entt::scoped_connection m_mapOpenConnection;
    entt::scoped_connection m_mapCloseConnection;
};
