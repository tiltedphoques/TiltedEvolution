#include <GameServer.h>

#include <Services/OverlayService.h>

#include <Messages/NotifyChatMessageBroadcast.h>
#include <Messages/SendChatMessageRequest.h>
#include <Messages/PlayerDialogueRequest.h>
#include <Messages/NotifyPlayerDialogue.h>
#include <Messages/TeleportRequest.h>
#include <Messages/NotifyTeleport.h>

#include "Game/Player.h"

#include <regex>

OverlayService::OverlayService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld)
{
    m_chatMessageConnection = aDispatcher.sink<PacketEvent<SendChatMessageRequest>>().connect<&OverlayService::HandleChatMessage>(this);
    m_playerDialogueConnection = aDispatcher.sink<PacketEvent<PlayerDialogueRequest>>().connect<&OverlayService::OnPlayerDialogue>(this);
    m_teleportConnection = aDispatcher.sink<PacketEvent<TeleportRequest>>().connect<&OverlayService::OnTeleport>(this);
}

void OverlayService::HandleChatMessage(const PacketEvent<SendChatMessageRequest>& acMessage) const noexcept
{
    NotifyChatMessageBroadcast notifyMessage{};
    notifyMessage.PlayerName = acMessage.pPlayer->GetUsername();

    std::regex escapeHtml{"<[^>]+>\\s+(?=<)|<[^>]+>"};
    notifyMessage.ChatMessage = std::regex_replace(acMessage.Packet.ChatMessage, escapeHtml, "");

    GameServer::Get()->SendToPlayers(notifyMessage);
}

void OverlayService::OnPlayerDialogue(const PacketEvent<PlayerDialogueRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    NotifyPlayerDialogue notify{};
    notify.Name = acMessage.pPlayer->GetUsername();

    std::regex escapeHtml{"<[^>]+>\\s+(?=<)|<[^>]+>"};
    notify.Text = std::regex_replace(message.Text, escapeHtml, "");

    auto& party = acMessage.pPlayer->GetParty();
    GameServer::Get()->SendToParty(notify, party);
}

void OverlayService::OnTeleport(const PacketEvent<TeleportRequest>& acMessage) const noexcept
{
    Player* pTargetPlayer = m_world.GetPlayerManager().GetById(acMessage.Packet.PlayerId);
    if (!pTargetPlayer)
        return;

    NotifyTeleport response{};

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

    acMessage.pPlayer->Send(response);
}

#if 0
#include <Components.h>
#include <Events/PlayerEnterWorldEvent.h>

void OverlayService::HandlePlayerJoin(const PlayerEnterWorldEvent& acEvent) const noexcept
{
    const Script::Player cPlayer(acEvent.Entity, m_world);

    auto& playerComponent = m_world.get<PlayerComponent>(cPlayer.GetEntityHandle());

    spdlog::info("[SERVER] PlayerId: {} - ConnectionId: {}", cPlayer.GetId(), playerComponent.ConnectionId);
    if (playerComponent.Character)
    {
        spdlog::info("[SERVER] CharacterId: {}", playerComponent.Character.value());
    }

    //TODO 
    //Send netid, username and default level to all client except to the one that just joined
}
#endif
