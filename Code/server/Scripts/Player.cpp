#include <stdafx.h>

#include <Scripts/Player.h>
#include <Scripts/Npc.h>
#include <Scripts/Quest.h>
#include <Scripts/Party.h>

#include <World.h>
#include <Components.h>

#include <Services/PartyService.h>

namespace Script
{
    Player::Player(entt::entity aEntity, World& aWorld)
        : EntityHandle(aEntity, aWorld)
    {}

    const Vector<String>& Player::GetMods() const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);
        return playerComponent.Mods;
    }

    const String& Player::GetIp() const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);
        return playerComponent.Endpoint;
    }

    const String& Player::GetName() const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);
        return playerComponent.Username;
    }

    const uint64_t Player::GetDiscordId() const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);
        return playerComponent.DiscordId;
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
        auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

        return 0.f;
    }

    bool Player::AddComponent(sol::object aObject) const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);

        // If the player has a character we add the component to the child, makes no sense to add to a player
        if(playerComponent.Character)
        {
            const Script::Npc npc(*playerComponent.Character, *m_pWorld);
            return npc.AddComponent(aObject);
        }

        return false;
    }

    bool Player::HasMod(const std::string& aModName) const noexcept
    {
        auto& mods = m_pWorld->get<PlayerComponent>(m_entity).Mods;

        auto it = std::find_if(mods.begin(), mods.end(), [&aModName](const String& it) {
            // SOL allocator mismatch crash workaround.
            return std::strcmp(it.c_str(), aModName.c_str()) == 0;
        });

        return it != mods.end();
    }

    bool Player::RemoveQuest(uint32_t aformId)
    {
        auto* pQuestComponent = m_pWorld->try_get<QuestLogComponent>(m_entity);
        if (!pQuestComponent)
            return false;

        auto& questService = m_pWorld->GetQuestService();
        auto& entries = pQuestComponent->QuestContent.Entries;

        auto it = std::find_if(entries.begin(), entries.end(), [&aformId](const auto& e) 
        { 
            return e.Id.BaseId == aformId; 
        });

        if (it != entries.end())
        {
            // dispatch the message
            questService.StartStopQuest(m_entity, it->Id, true);
            entries.erase(it);

            return true;
        }

        return false;
    }

    sol::optional<Quest> Player::AddQuest(const std::string aModName, uint32_t aformId)
    {
        // safeguard in the case that the player calls GetQuests in the onPlayerJoin event
        // where our questlog representation hasn't been created yet.
        auto* pQuestComponent = m_pWorld->try_get<QuestLogComponent>(m_entity);
        if (!pQuestComponent)
            return sol::nullopt;

        if (aModName.find(".esm") == std::string::npos && 
            aModName.find(".esl") == std::string::npos)
            return sol::nullopt;

        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);
        auto& playerMods = playerComponent.Mods;

        auto it = std::find_if(playerMods.begin(), playerMods.end(), [&aModName](const auto& it) 
        {
            return std::strcmp(it.c_str(), aModName.c_str()) == 0;
        });

        if (it == playerMods.end())
            return std::nullopt;

        auto index = it - playerMods.begin();

        // this should probably be an assert instead
        if (index < 0ll || index > playerMods.size())
            return std::nullopt;

        // if we not set a baseid we should set it to temporary.. :)
        // that way the system knows...

        GameId questId;
        questId.BaseId = aformId & 0xFFFFFF;
        questId.ModId = static_cast<uint32_t>(playerComponent.ModIds[index]);

        auto& questService = m_pWorld->GetQuestService();
        auto& entries = pQuestComponent->QuestContent.Entries;

        // send it to the recipient
        if (!questService.StartStopQuest(m_entity, questId, false))
            return sol::nullopt;

        auto& newQuest = entries.emplace_back();
        newQuest.Id = questId;
        newQuest.Stage = 0;

        return Quest(aformId, 0, *m_pWorld);
    }

    sol::optional<Vector<Quest>> Player::GetQuests() const noexcept
    {
        if (auto* pQuestComponent = m_pWorld->try_get<QuestLogComponent>(m_entity))
        {
            auto& entries = pQuestComponent->QuestContent.Entries;

            Vector<Quest> scriptQuests;
            scriptQuests.reserve(entries.size());

            for (const auto& it : entries)
            {
                scriptQuests.push_back(Quest(it.Id.BaseId, it.Stage, *m_pWorld));
            }

            return std::move(scriptQuests);
        }

        return sol::nullopt;
    }

    sol::optional<Party> Player::GetParty() const noexcept
    {
        if (auto* pPartyComponent = m_pWorld->try_get<PartyComponent>(m_entity))
        {
            if (pPartyComponent->JoinedPartyId)
            {
                return Party(*pPartyComponent->JoinedPartyId, *m_pWorld);
            }
        }

        return sol::nullopt;
    }
}
