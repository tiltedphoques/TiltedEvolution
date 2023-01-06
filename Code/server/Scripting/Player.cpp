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
#include <Messages/NotifyQuestUpdate.h>

namespace Script
{
static Vector<String> kNullVecString{};
static String kNullString{};
static glm::vec3 kNullVec{};

Vector<String> Player::GetMods() const
{
    Vector<String> mods{};

    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        for (const auto& [name, _] : pPlayerObject->GetMods())
        {
            mods.push_back(name);
        }
    }

    return mods;
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
        return pPlayerObject->GetMods().contains(String(aModName));
    }

    return false;
}

sol::optional<Quest> Player::AddQuest(const std::string aModName, uint32_t aFormId)
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        auto& mods = pPlayerObject->GetMods();
        auto modId = mods.find(String(aModName));
        if (modId != mods.end())
        {
            GameId gameId{};
            gameId.ModId = modId->second;
            gameId.BaseId = aFormId & 0x00FFFFFF;

            auto& entry = pPlayerObject->GetQuestLogComponent().QuestContent.Entries.emplace_back();

            NotifyQuestUpdate notify{};
            notify.Id = gameId;
            notify.Stage = 0; // TODO: stage 0 is probably not always the actual start of the quest
            notify.Status = NotifyQuestUpdate::Started;

            pPlayerObject->Send(notify);

            return Quest(gameId, 0, m_pWorld);
        }
    }

    return std::nullopt;
}

bool Player::UpdateQuest(const Quest& aQuest)
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        bool hasMod = false;
        for (const auto& [_, id] : pPlayerObject->GetMods())
        {
            if (id == aQuest.GetId().ModId)
            {
                hasMod = true;
                break;
            }
        }

        if (hasMod)
        {
            NotifyQuestUpdate notify{};
            notify.Id = aQuest.GetId();
            notify.Stage = aQuest.GetStage();
            notify.Status = NotifyQuestUpdate::StageUpdate;

            pPlayerObject->Send(notify);

            return true;
        }
    }

    return false;
}

bool Player::RemoveQuest(std::string aModName, uint32_t aFormId)
{
    if (auto* pPlayerObject = PlayerManager::Get()->GetById(m_playerObjectRefId))
    {
        auto& mods = pPlayerObject->GetMods();
        auto modId = mods.find(String(aModName));
        if (modId != mods.end())
        {
            GameId gameId{};
            gameId.ModId = modId->second;
            gameId.BaseId = aFormId & 0x00FFFFFF;

            auto& entry = pPlayerObject->GetQuestLogComponent().QuestContent.Entries.emplace_back();

            NotifyQuestUpdate notify{};
            notify.Id = gameId;
            notify.Status = NotifyQuestUpdate::Stopped;

            pPlayerObject->Send(notify);

            return true;
        }
    }

    return false;
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
