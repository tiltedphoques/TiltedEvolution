
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
    // type["SendPacket"]
}
} // namespace Script
