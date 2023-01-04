#include <stdafx.h>

#include <Scripting/Npc.h>
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
static Inventory kNullInventory{};

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

const glm::vec3& Player::GetPosition() const
{
    if (!GameServer::Get()->GetWorld().valid(m_entity))
    {
        return kNullVec;
    }

    auto* movementComponent = GameServer::Get()->GetWorld().try_get<MovementComponent>(m_entity);
    if (!movementComponent)
    {
        return kNullVec;
    }

    return movementComponent->Position;
}

const glm::vec3& Player::GetRotation() const
{
    if (!m_pWorld->valid(m_entity))
    {
        return kNullVec;
    }

    auto* movementComponent = m_pWorld->try_get<MovementComponent>(m_entity);
    if (!movementComponent)
    {
        return kNullVec;
    }

    return movementComponent->Rotation;
}

float Player::GetSpeed() const
{
    return 0.f;
}

const Inventory& Player::GetInventory() const
{
    auto* pPlayer = PlayerManager::Get()->GetById(m_playerObjectRefId);
    if (!pPlayer)
        return kNullInventory;

    auto character = pPlayer->GetCharacter();
    if (!character)
        return kNullInventory;

    const auto* inventoryComponent = GameServer::Get()->GetWorld().try_get<InventoryComponent>(*character);
    if (!inventoryComponent)
        return kNullInventory;

    return inventoryComponent->Content;
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
