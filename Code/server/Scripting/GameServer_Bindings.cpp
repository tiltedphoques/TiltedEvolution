
#include "GameServer.h"
#include <Messages/NotifyChatMessageBroadcast.h>
#include <regex>

namespace Script
{
void CreateGameServerBindings(sol::state_view aState)
{
    auto table = aState.new_usertype<GameServer>("GameServer", sol::meta_function::construct, sol::no_constructor);
    table["get"] = []() { return GameServer::Get(); };
    table["Kill"] = &GameServer::Kill;
    table["Kick"] = &GameServer::Kick;

    table["SendChatMessage"] = [](GameServer& aSelf, ConnectionId_t aConnectionId, const std::string& acMessage) {
        NotifyChatMessageBroadcast notifyMessage{};

        std::regex escapeHtml{"<[^>]+>\\s+(?=<)|<[^>]+>"};
        notifyMessage.MessageType = ChatMessageType::kLocalChat;
        notifyMessage.PlayerName = "[Server]";
        notifyMessage.ChatMessage = std::regex_replace(acMessage, escapeHtml, "");
        GameServer::Get()->Send(aConnectionId, notifyMessage);
    };
    // table["SendPacket"]
}
} // namespace Script
