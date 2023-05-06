#include <Services/MapService.h>
#include <GameServer.h>

#include <Messages/NotifyRemoveWaypoint.h>
#include <Messages/NotifySetWaypoint.h>
#include <Messages/RequestRemoveWaypoint.h>
#include <Messages/RequestSetWaypoint.h>

MapService::MapService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
{
      m_playerSetWaypointConnection = 
          aDispatcher.sink<PacketEvent<RequestSetWaypoint>>().connect<&MapService::OnSetWaypointRequest>(this);
      m_playerRemoveWaypointConnection =
          aDispatcher.sink<PacketEvent<RequestRemoveWaypoint>>().connect<&MapService::OnRemoveWaypointRequest>(this);
}

void MapService::OnSetWaypointRequest(const PacketEvent<RequestSetWaypoint>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    NotifySetWaypoint notify{};
    notify.Position = message.Position;

    // TODO: send to players in party
    GameServer::Get()->SendToPlayers(notify, acMessage.pPlayer);
}

void MapService::OnRemoveWaypointRequest(const PacketEvent<RequestRemoveWaypoint>& acMessage) const noexcept
{
    NotifyRemoveWaypoint notify{};

    // TODO: send to players in party
    GameServer::Get()->SendToPlayers(notify, acMessage.pPlayer);
}

