#pragma once

#include <Scripts/EntityHandle.h>

struct QuestLogComponent;

namespace Script
{
    struct Quest;

    struct Player : EntityHandle
    {
        Player(entt::entity aEntity, World& aWorld);

        const Vector<String>& GetMods() const;
        const String& GetIp() const;
        const uint64_t GetDiscordId() const;

        [[nodiscard]] const Vector3<float>& GetPosition() const;
        [[nodiscard]] const Vector3<float>& GetRotation() const;
        [[nodiscard]] float GetSpeed() const;

        Player& operator=(const Player& acRhs)
        {
            EntityHandle::operator=(acRhs);

            return *this;
        }

        bool AddComponent(sol::object aObject) const override;
        bool HasMod(const std::string& aModName) const noexcept;

        bool RemoveQuest(uint32_t aformId);
        sol::optional<Quest> AddQuest(const std::string aModName, uint32_t aformId);
        sol::optional<Vector<Quest>> GetQuests() const noexcept;

        inline entt::entity GetEntityHandle() const { return m_entity; }
    };
}
