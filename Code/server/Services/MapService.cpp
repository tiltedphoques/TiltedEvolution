#include "Events/CharacterExteriorCellChangeEvent.h"
#include "Events/CharacterInteriorCellChangeEvent.h"
#include "Events/PlayerLeaveCellEvent.h"
#include <Events/UpdateEvent.h>

#include <GameServer.h>
#include <Services/CharacterService.h>
#include <Services/MapService.h>

#include <Messages/CharacterSpawnRequest.h>
#include <Messages/EnterExteriorCellRequest.h>
#include <Messages/EnterInteriorCellRequest.h>
#include <Messages/NotifyDelWaypoint.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/NotifyPlayerCellChanged.h>
#include <Messages/NotifyPlayerLevel.h>
#include <Messages/NotifyPlayerPosition.h>
#include <Messages/NotifyPlayerRespawn.h>
#include <Messages/NotifyRespawn.h>
#include <Messages/NotifySetWaypoint.h>
#include <Messages/PlayerLevelRequest.h>
#include <Messages/PlayerRespawnRequest.h>
#include <Messages/RequestDelWaypoint.h>
#include <Messages/RequestSetWaypoint.h>
#include <Messages/ShiftGridCellRequest.h>

MapService::MapService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld), 
      m_updateConnection(aDispatcher.sink<UpdateEvent>().connect<&MapService::OnUpdate>(this)),
      m_playerSetWaypointConnection(aDispatcher.sink<PacketEvent<RequestSetWaypoint>>().connect<&MapService::OnSetWaypointRequest>(this)),
      m_playerDelWaypointConnection(aDispatcher.sink<PacketEvent<RequestDelWaypoint>>().connect<&MapService::OnDelWaypointRequest>(this))
{
}

void MapService::OnUpdate(const UpdateEvent&) const noexcept
{
    ProcessPlayerPositionChanges();
}

void MapService::OnSetWaypointRequest(const PacketEvent<RequestSetWaypoint>& acMessage) const noexcept
{
    auto* pPlayer = acMessage.pPlayer;

    auto& message = acMessage.Packet;

    NotifySetWaypoint notify{};
    notify.Position = message.Position;

    GameServer::Get()->SendToPlayers(notify, acMessage.pPlayer);
}

void MapService::OnDelWaypointRequest(const PacketEvent<RequestDelWaypoint>& acMessage) const noexcept
{
    NotifyDelWaypoint notify{};

    GameServer::Get()->SendToPlayers(notify, acMessage.pPlayer);
}

void MapService::ProcessPlayerPositionChanges() const noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 500ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    TiltedPhoques::Vector<NotifyPlayerPosition> messages;

    // TODO: optimize this so that all player updates are sent in one message
    for (Player* pPlayer : m_world.GetPlayerManager())
    {
        if (!pPlayer->GetCharacter())
            continue;

        auto character = *pPlayer->GetCharacter();
        auto* movementComponent = m_world.try_get<MovementComponent>(character);
        if (!movementComponent)
            continue;

        auto& message = messages.emplace_back();
        message.PlayerId = pPlayer->GetId();

        message.Position = movementComponent->Position;
    }

    for (auto& message : messages)
    {
        Player* pPlayer = m_world.GetPlayerManager().GetById(message.PlayerId);
        GameServer::Get()->SendToPlayers(message, pPlayer);
    }
}
