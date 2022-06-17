#pragma once

#include <Events/PacketEvent.h>

struct World;
struct ShiftGridCellRequest;
struct EnterInteriorCellRequest;
struct EnterExteriorCellRequest;
struct PlayerRespawnRequest;
struct RequestSetWaypoint;
struct RequestDelWaypoint;
struct PlayerLevelRequest;
struct UpdateEvent;

/**
* @brief Handles player specific actions that might change the information needed by other clients about that player.
*/
struct PlayerService
{
    PlayerService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~PlayerService() noexcept = default;

    TP_NOCOPYMOVE(PlayerService);

protected:

    void OnUpdate(const UpdateEvent& acEvent) const noexcept;
    void HandleGridCellShift(const PacketEvent<ShiftGridCellRequest>& acMessage) const noexcept;
    void HandleExteriorCellEnter(const PacketEvent<EnterExteriorCellRequest>& acMessage) const noexcept;
    void HandleInteriorCellEnter(const PacketEvent<EnterInteriorCellRequest>& acMessage) const noexcept;
    void OnPlayerRespawnRequest(const PacketEvent<PlayerRespawnRequest>& acMessage) const noexcept;
    void OnPlayerLevelRequest(const PacketEvent<PlayerLevelRequest>& acMessage) const noexcept;
    void OnSetWaypointRequest(const PacketEvent<RequestSetWaypoint>& acMessage) const noexcept;
    void OnDelWaypointRequest(const PacketEvent<RequestDelWaypoint>& acMessage) const noexcept;
    void ProcessPlayerPositionChanges() const noexcept;

private:

    World& m_world;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_gridCellShiftConnection;
    entt::scoped_connection m_exteriorCellEnterConnection;
    entt::scoped_connection m_interiorCellEnterConnection;
    entt::scoped_connection m_playerRespawnConnection;
    entt::scoped_connection m_playerLevelConnection;
    entt::scoped_connection m_playerSetWaypointConnection;
    entt::scoped_connection m_playerDelWaypointConnection;
};
