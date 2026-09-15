#include <Services/CommandService.h>

#include <Components.h>
#include <GameServer.h>
#include <World.h>

#include <Messages/SetTimeCommandRequest.h>
#include <Messages/NotifySetTimeResult.h>
#include <Messages/RequestTimeSkip.h>
#include <Messages/TeleportCommandRequest.h>
#include <Messages/TeleportCommandResponse.h>

#include <Setting.h>

namespace
{
Console::Setting bAnnounceServer{"LiveServices:bAnnounceServer", "Whether to list the server on the public server list", false};
}

CommandService::CommandService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
{
    m_setTimeConnection = aDispatcher.sink<PacketEvent<SetTimeCommandRequest>>().connect<&CommandService::OnSetTimeCommand>(this);
    m_teleportConnection = aDispatcher.sink<PacketEvent<TeleportCommandRequest>>().connect<&CommandService::OnTeleportCommandRequest>(this);
    m_timeSkipConnection = aDispatcher.sink<PacketEvent<RequestTimeSkip>>().connect<&CommandService::OnTimeSkipRequest>(this);
}

bool CommandService::CanChangeTime(const Player* apPlayer) const noexcept
{
    // Admin override: always allowed
    for (const auto session : GameServer::Get()->GetAdminSessions())
    {
        const Player* pAdmin = PlayerManager::Get()->GetByConnectionId(session);
        if (pAdmin && pAdmin->GetId() == apPlayer->GetId())
            return true;
    }

    // Party leader allowed on private servers only
    return m_world.GetPartyService().IsPlayerLeader(apPlayer) && !bAnnounceServer;
}

void CommandService::OnTimeSkipRequest(const PacketEvent<RequestTimeSkip>& acMessage) const noexcept
{
    const float cHours = acMessage.Packet.Hours;

    if (!CanChangeTime(acMessage.pPlayer))
    {
        spdlog::debug("Ignoring time skip of {} hours from player {:X}: no permission", cHours, acMessage.pPlayer->GetId());
        return;
    }

    if (m_world.GetCalendarService().AdvanceTime(cHours))
        spdlog::info("Player {:X} slept or waited: advanced the shared time by {} hours", acMessage.pPlayer->GetId(), cHours);
    else
        spdlog::warn("Rejected time skip of {} hours from player {:X}", cHours, acMessage.pPlayer->GetId());
}

void CommandService::OnSetTimeCommand(const PacketEvent<SetTimeCommandRequest>& acMessage) const noexcept
{
    NotifySetTimeResult response{};

    const auto cPlayerId = static_cast<uint32_t>(acMessage.Packet.PlayerId);

    // Admin override: always allowed
    for (const auto session : GameServer::Get()->GetAdminSessions())
    {
        if (PlayerManager::Get()->GetByConnectionId(session)->GetId() == cPlayerId)
        {
            const auto cHours = static_cast<int>(acMessage.Packet.Hours);
            const auto cMinutes = static_cast<int>(acMessage.Packet.Minutes);

            m_world.GetCalendarService().SetTime(cHours, cMinutes, m_world.GetCalendarService().GetTimeScale());

            response.Result = NotifySetTimeResult::SetTimeResult::kSuccess;
            acMessage.pPlayer->Send(response);

            return;
        }
    }

    // Party leader allowed on private servers only
    const auto* pPartyService = &m_world.GetPartyService();
    if (pPartyService->IsPlayerLeader(acMessage.pPlayer) && !bAnnounceServer)
    {
        const auto cHours = static_cast<int>(acMessage.Packet.Hours);
        const auto cMinutes = static_cast<int>(acMessage.Packet.Minutes);

        m_world.GetCalendarService().SetTime(cHours, cMinutes, m_world.GetCalendarService().GetTimeScale());

        response.Result = NotifySetTimeResult::SetTimeResult::kSuccess;
        acMessage.pPlayer->Send(response);

        return;
    }

    response.Result = NotifySetTimeResult::SetTimeResult::kNoPermission;
    acMessage.pPlayer->Send(response);
}

void CommandService::OnTeleportCommandRequest(const PacketEvent<TeleportCommandRequest>& acMessage) const noexcept
{
    Player* pTargetPlayer = nullptr;
    for (Player* pPlayer : m_world.GetPlayerManager())
    {
        if (pPlayer->GetUsername() == acMessage.Packet.TargetPlayer)
            pTargetPlayer = pPlayer;
    }

    TeleportCommandResponse response{};
    if (pTargetPlayer)
    {
        auto character = pTargetPlayer->GetCharacter();
        if (character)
        {
            const auto* pMovementComponent = m_world.try_get<MovementComponent>(*character);
            if (pMovementComponent)
            {
                const auto& cellComponent = pTargetPlayer->GetCellComponent();
                response.CellId = cellComponent.Cell;
                response.Position = pMovementComponent->Position;
                response.WorldSpaceId = cellComponent.WorldSpaceId;
            }
        }
    }

    acMessage.pPlayer->Send(response);
}
