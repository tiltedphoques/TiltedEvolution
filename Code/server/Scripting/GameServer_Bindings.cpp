
#include "GameServer.h"

#include <Messages/NotifyChatMessageBroadcast.h>
#include <regex>

namespace Script
{
void CreateGameServerBindings(sol::state_view aState)
{
    auto type = aState.new_usertype<GameServer>("GameServer", sol::meta_function::construct, sol::no_constructor);
    type["get"] = []() { return GameServer::Get(); };
    type["Kill"] = &GameServer::Kill;
    type["Kick"] = &GameServer::Kick;
    type["GetTick"] = &GameServer::GetTick;

    type["SendChatMessage"] = [](GameServer& aSelf, ConnectionId_t aConnectionId, const std::string& acMessage) {
        NotifyChatMessageBroadcast notifyMessage{};

        std::regex escapeHtml{"<[^>]+>\\s+(?=<)|<[^>]+>"};
        notifyMessage.MessageType = ChatMessageType::kLocalChat;
        notifyMessage.PlayerName = "[Server]";
        notifyMessage.ChatMessage = std::regex_replace(acMessage, escapeHtml, "");
        GameServer::Get()->Send(aConnectionId, notifyMessage);
    };
    type["SendGlobalChatMessage"] = [](GameServer& aSelf, const std::string& acMessage) {
        NotifyChatMessageBroadcast notifyMessage{};

        std::regex escapeHtml{"<[^>]+>\\s+(?=<)|<[^>]+>"};
        notifyMessage.MessageType = ChatMessageType::kGlobalChat;
        notifyMessage.PlayerName = "[Server]";
        notifyMessage.ChatMessage = std::regex_replace(acMessage, escapeHtml, "");
        GameServer::Get()->SendToPlayers(notifyMessage);
    };
    type["SetTime"] = [](GameServer& aSelf, int aHours, int aMinutes, float aScale = GameServer::Get()->GetWorld().GetCalendarService().GetTimeScale()) {
        aHours = std::max(0, std::min(aHours, 23));
        aMinutes = std::max(0, std::min(aMinutes, 59));
        aScale = std::max(1.0f, std::min(aScale, 100.0f));

        GameServer::Get()->GetWorld().GetCalendarService().SetTime(aHours, aMinutes, aScale);
    };
    // type["SendPacket"]
}
} // namespace Script
