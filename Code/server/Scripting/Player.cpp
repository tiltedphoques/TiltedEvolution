#include <stdafx.h>

#include <Scripting/Player.h>
#include <Scripting/Npc.h>
#include <Scripting/Quest.h>
#include <Scripting/Party.h>

#include <World.h>
#include <Components.h>

#include <Game/PlayerManager.h>

namespace Script
{
const Vector<String>& Player::GetMods() const
{
    return {};
}

const String& Player::GetIp() const
{
    return {};
}

const String& Player::GetName() const
{
    return PlayerManager::Get()->GetById(m_playerObjectRefId)->GetUsername();
}

const uint64_t Player::GetDiscordId() const
{
    return PlayerManager::Get()->GetById(m_playerObjectRefId)->GetDiscordId();
}

const glm::vec3& Player::GetPosition() const
{
    auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);
    return movementComponent.Position;
}

const glm::vec3& Player::GetRotation() const
{
    auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);
    return movementComponent.Rotation;
}

float Player::GetSpeed() const
{
    return 0.f;
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

sol::optional<Vector<Quest>> Player::GetQuests() const noexcept
{
    return sol::nullopt;
}

void Player::SendChatMessage(const String& acMessage) noexcept
{
    
}

sol::optional<Party> Player::GetParty() const noexcept
{
    return sol::nullopt;
}

} // namespace Script
