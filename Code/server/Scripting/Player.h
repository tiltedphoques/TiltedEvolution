#pragma once

#include <Scripting/EntityHandle.h>

struct QuestLogComponent;

namespace Script
{
struct Quest;
struct Party;

struct Player : EntityHandle
{
    Player(entt::entity aEntity, World& aWorld);

    const Vector<String>& GetMods() const;
    const String& GetIp() const;
    const String& GetName() const;
    const uint64_t GetDiscordId() const;

    [[nodiscard]] const glm::vec3& GetPosition() const;
    [[nodiscard]] const glm::vec3& GetRotation() const;
    [[nodiscard]] float GetSpeed() const;

    Player& operator=(const Player& acRhs)
    {
        EntityHandle::operator=(acRhs);

        return *this;
    }

    bool HasMod(const std::string& aModName) const noexcept;

    bool RemoveQuest(uint32_t aformId);
    sol::optional<Quest> AddQuest(std::string aModName, uint32_t aformId);
    sol::optional<Vector<Quest>> GetQuests() const noexcept;
    sol::optional<Party> GetParty() const noexcept;

    inline entt::entity GetEntityHandle() const { return m_entity; }
};
} // namespace Script
