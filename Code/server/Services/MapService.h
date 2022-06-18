#pragma once

#include <Events/PacketEvent.h>

struct World;
struct RequestSetWaypoint;
struct RequestDelWaypoint;
struct PlayerLevelRequest;
struct UpdateEvent;

/**
 * @brief Handles player specific actions that might change the information needed by other clients about that player.
 */
struct MapService
{
    MapService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~MapService() noexcept = default;

    TP_NOCOPYMOVE(MapService);

  protected:
    void OnUpdate(const UpdateEvent& acEvent) const noexcept;
    void OnSetWaypointRequest(const PacketEvent<RequestSetWaypoint>& acMessage) const noexcept;
    void OnDelWaypointRequest(const PacketEvent<RequestDelWaypoint>& acMessage) const noexcept;
    void ProcessPlayerPositionChanges() const noexcept;

  private:
    World& m_world;

    entt::scoped_connection m_playerSetWaypointConnection;
    entt::scoped_connection m_playerDelWaypointConnection;
    entt::scoped_connection m_updateConnection;
};
