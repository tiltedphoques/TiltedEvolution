#pragma once

#include <Scripting/EntityHandle.h>

#include <Structs/Inventory.h>

struct QuestLogComponent;

// non Script player
struct Player;

namespace Script
{
struct Quest;
struct Party;
struct Actor;

struct Player : EntityHandle
{
    inline Player(uint32_t m_playerObjectRefId /*playermanager id*/, entt::entity aEntity, World& aWorld)
        : m_playerObjectRefId(m_playerObjectRefId)
        , EntityHandle(aEntity, aWorld)
    {
    }

    Vector<String> GetMods() const;
    const String& GetIp() const;
    const String& GetName() const;
    const uint64_t GetDiscordId() const;
    sol::optional<Script::Actor> GetActor() const;
    Vector<Quest> GetQuests() const;

    Player& operator=(const Player& acRhs)
    {
        EntityHandle::operator=(acRhs);

        return *this;
    }

    bool HasMod(const std::string& aModName) const noexcept;

    sol::optional<Quest> AddQuest(std::string aModName, uint32_t aformId);
    bool UpdateQuest(const Quest& aQuest);
    bool RemoveQuest(uint32_t aformId);

    bool Kick() noexcept;
    bool SendChatMessage(const std::string& acMessage) noexcept;

    sol::optional<Party> GetParty() const noexcept;

    inline entt::entity GetEntityHandle() const { return m_entity; }

private:
    uint32_t m_playerObjectRefId = 0;
};
} // namespace Script
