#include <stdafx.h>

#include <Scripting/Actor.h>
#include <Scripting/Party.h>
#include <Scripting/Player.h>
#include <Scripting/Quest.h>

#include <Components.h>
#include <World.h>

#include <Game/PlayerManager.h>
#include <GameServer.h>

#include <regex>
#include <Messages/NotifyChatMessageBroadcast.h>

namespace Script
{
static Vector<String> kNullVecString{};
static String kNullString{};
static glm::vec3 kNullVec{};

const Vector<String>& Player::GetMods() const
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        return pPlayerObject->GetMods();
    }

    return kNullVecString;
}

const String& Player::GetIp() const
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        return pPlayerObject->GetEndPoint();
    }
    return kNullString;
}

const String& Player::GetName() const
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        return pPlayerObject->GetUsername();
    }
    return kNullString;
}

const uint64_t Player::GetDiscordId() const
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        return pPlayerObject->GetDiscordId();
    }
    return 0;
}

sol::optional<Script::Actor> Player::GetActor() const
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        if (auto character = pPlayerObject->GetCharacter())
        {
            return Script::Actor(*character, *m_pWorld);
        }
    }

    return sol::nullopt;
}

Vector<Quest> Player::GetQuests() const
{
    Vector<Quest> quests{};

    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        for (const auto& quest : pPlayerObject->GetQuestLogComponent().QuestContent.Entries)
            quests.push_back(Quest(quest.Id, quest.Stage, m_pWorld));
    }

    return quests;
}

bool Player::HasMod(const std::string& aModName) const noexcept
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        for (const auto& modName : pPlayerObject->GetMods())
        {
            std::string mod(modName.c_str());
            if (mod == aModName)
                return true;
        }
    }

    return false;
}

bool Player::RemoveQuest(uint32_t aformId)
{
    return false;
}

sol::optional<Quest> Player::AddQuest(const std::string aModName, uint32_t aformId)
{
    return std::nullopt;
}

bool Player::SendChatMessage(const std::string& acMessage) noexcept
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        NotifyChatMessageBroadcast notifyMessage{};

        std::regex escapeHtml{"<[^>]+>\\s+(?=<)|<[^>]+>"};
        notifyMessage.MessageType = ChatMessageType::kLocalChat;
        notifyMessage.PlayerName = "[Server]";
        notifyMessage.ChatMessage = std::regex_replace(acMessage, escapeHtml, "");
        GameServer::Get()->Send(pPlayerObject->GetConnectionId(), notifyMessage);
        return true;
    }
    return false;
}

bool Player::Kick() noexcept
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        GameServer::Get()->Kick(pPlayerObject->GetConnectionId());
        return true;
    }
    return false;
}

sol::optional<Party> Player::GetParty() const noexcept
{
    return sol::nullopt;
}

} // namespace Script
