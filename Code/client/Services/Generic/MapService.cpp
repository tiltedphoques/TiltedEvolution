#include <Services/MapService.h>

#include <World.h>

#include <Events/SetWaypointEvent.h>
#include <Events/RemoveWaypointEvent.h>
#include <Messages/NotifySetWaypoint.h>
#include <Messages/NotifyRemoveWaypoint.h>
#include <Messages/RequestSetWaypoint.h>
#include <Messages/RequestRemoveWaypoint.h>

#include <PlayerCharacter.h>

MapService::MapService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld), m_dispatcher(aDispatcher), m_transport(aTransport)
{
    m_playerNotifySetWaypointConnection =
        m_dispatcher.sink<NotifySetWaypoint>().connect<&MapService::OnNotifySetWaypoint>(this);
    m_playerNotifyRemoveWaypointConnection =
        m_dispatcher.sink<NotifyRemoveWaypoint>().connect<&MapService::OnNotifyRemoveWaypoint>(this);
    m_playerSetWaypointConnection =
        m_dispatcher.sink<SetWaypointEvent>().connect<&MapService::OnSetWaypoint>(this);
    m_playerRemoveWaypointConnection =
        m_dispatcher.sink<RemoveWaypointEvent>().connect<&MapService::OnRemoveWaypoint>(this);
}

void MapService::OnSetWaypoint(const SetWaypointEvent& acMessage) noexcept
{
    if (!m_transport.IsConnected())
        return;

    RequestSetWaypoint request{};
    request.Position = acMessage.Position;

    ModSystem& modSystem = m_world.Get().GetModSystem();
    modSystem.GetServerModId(acMessage.WorldSpaceFormID, request.WorldSpaceFormID);

    m_transport.Send(request);
}

void MapService::OnRemoveWaypoint(const RemoveWaypointEvent& acMessage) noexcept
{
    if (!m_transport.IsConnected())
        return;

    RequestRemoveWaypoint request{};
    m_transport.Send(request);
}

void MapService::OnNotifySetWaypoint(const NotifySetWaypoint& acMessage) noexcept
{
    NiPoint3 pos{};
    pos.x = acMessage.Position.x;
    pos.y = acMessage.Position.y;
    pos.z = acMessage.Position.z;

    ModSystem& modSystem = m_world.Get().GetModSystem();
    const uint32_t cWorldSpaceID = modSystem.GetGameId(acMessage.WorldSpaceFormID);
    TESWorldSpace* pWorldSpace = Cast<TESWorldSpace>(TESForm::GetById(cWorldSpaceID));

    PlayerCharacter::Get()->SetWaypoint(&pos, pWorldSpace);
}

void MapService::OnNotifyRemoveWaypoint(const NotifyRemoveWaypoint& acMessage) noexcept
{
    PlayerCharacter::Get()->RemoveWaypoint();
}

