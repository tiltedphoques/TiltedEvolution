#include <GameServer.h>

#include <Services/OverlayService.h>

#include <Messages/NotifyChatMessageBroadcast.h>
#include <Messages/SendChatMessageRequest.h>
#include <Messages/PlayerDialogueRequest.h>
#include <Messages/NotifyPlayerDialogue.h>
#include <Messages/TeleportRequest.h>
#include <Messages/NotifyTeleport.h>
#include <Messages/RequestPlayerHealthUpdate.h>
#include <Messages/NotifyPlayerHealthUpdate.h>

#include "Game/Player.h"

#include <regex>

OverlayService::OverlayService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld)
{
    m_chatMessageConnection = aDispatcher.sink<PacketEvent<SendChatMessageRequest>>().connect<&OverlayService::HandleChatMessage>(this);
    m_playerDialogueConnection = aDispatcher.sink<PacketEvent<PlayerDialogueRequest>>().connect<&OverlayService::OnPlayerDialogue>(this);
    m_teleportConnection = aDispatcher.sink<PacketEvent<TeleportRequest>>().connect<&OverlayService::OnTeleport>(this);
    m_playerHealthConnection = aDispatcher.sink<PacketEvent<RequestPlayerHealthUpdate>>().connect<&OverlayService::OnPlayerHealthUpdate>(this);
}

// maybe just gatther these into one method, which will sort out the targets
void OverlayService::HandleChatMessage(const PacketEvent<SendChatMessageRequest>& acMessage) const noexcept
{
    NotifyChatMessageBroadcast notifyMessage{};

    std::regex escapeHtml{"<[^>]+>\\s+(?=<)|<[^>]+>"};
    notifyMessage.MessageType = acMessage.Packet.MessageType;
    notifyMessage.PlayerName = std::regex_replace(acMessage.pPlayer->GetUsername(), escapeHtml, "");
    notifyMessage.ChatMessage = std::regex_replace(acMessage.Packet.ChatMessage, escapeHtml, "");

    switch (notifyMessage.MessageType)
    {
    case (kGlobalChat):
        GameServer::Get()->SendToPlayers(notifyMessage);
        break;

    case (kSystemMessage):
        spdlog::error("PlayerId {} attempted to send a System Message.", acMessage.pPlayer->GetId());
        break;

    case (kPlayerDialogue):
        GameServer::Get()->SendToParty(notifyMessage, acMessage.pPlayer->GetParty(), acMessage.pPlayer);
        break;

    default:
        spdlog::error("{} is not a known MessageType", static_cast<uint64_t>(notifyMessage.MessageType));
        break;
    }
    
}

void OverlayService::OnPlayerDialogue(const PacketEvent<PlayerDialogueRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    NotifyPlayerDialogue notify{};

    std::regex escapeHtml{"<[^>]+>\\s+(?=<)|<[^>]+>"};
    notify.Name = std::regex_replace(acMessage.pPlayer->GetUsername(), escapeHtml, "");
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

void OverlayService::OnPlayerHealthUpdate(const PacketEvent<RequestPlayerHealthUpdate>& acMessage) const noexcept
{
    NotifyPlayerHealthUpdate notify{};
    notify.PlayerId = acMessage.pPlayer->GetId();
    notify.Percentage = acMessage.Packet.Percentage;

    GameServer::Get()->SendToParty(notify, acMessage.pPlayer->GetParty(), acMessage.GetSender());
}
