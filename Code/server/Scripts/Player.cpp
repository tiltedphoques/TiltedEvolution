

#include <Scripts/Npc.h>
#include <Scripts/Party.h>
#include <Scripts/Player.h>
#include <Scripts/Quest.h>

#include <Components.h>
#include <World.h>

namespace Script
{
static glm::vec3 g_nullVec{};

Player::Player(entt::entity aEntity, World& aWorld) : EntityHandle(aEntity, aWorld)
{
}

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
    return {};
}

const uint64_t Player::GetDiscordId() const
{
    return {};
}

const glm::vec3& Player::GetPosition() const
{
    return g_nullVec;
}

const glm::vec3& Player::GetRotation() const
{
    return g_nullVec;
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

sol::optional<Party> Player::GetParty() const noexcept
{
    return sol::nullopt;
}

} // namespace Script
