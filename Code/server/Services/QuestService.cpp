
#include <GameServer.h>
#include <Components.h>

#include <World.h>
#include <Services/QuestService.h>

#include <Messages/RequestQuestUpdate.h>
#include <Messages/NotifyQuestUpdate.h>

#include <Scripts/Player.h>
#include <Scripts/Quest.h>

QuestService::QuestService(World& aWorld, entt::dispatcher& aDispatcher) : m_world(aWorld)
{
    m_questUpdateConnection =
        aDispatcher.sink<PacketEvent<RequestQuestUpdate>>().connect<&QuestService::HandleQuestChanges>(this);
}

void QuestService::HandleQuestChanges(const PacketEvent<RequestQuestUpdate>& acMessage) noexcept
{
    auto view = m_world.view<PlayerComponent, QuestLogComponent>();
    const auto it = std::find_if(view.begin(), view.end(),
                           [view, connectionId = acMessage.ConnectionId](auto entity) 
    {
        return view.get<PlayerComponent>(entity).ConnectionId == connectionId;
    });

    const auto& message = acMessage.Packet;

    if (it == view.end())
    {
        spdlog::error("Quest {:x} is not associated with connection {:x}", message.Id.BaseId,
                      acMessage.ConnectionId);
        return;
    }

    auto& questComponent = view.get<QuestLogComponent>(*it);
    auto& entries = questComponent.QuestContent.Entries;

    auto questIt = std::find_if(entries.begin(), entries.end(), [&message](const auto& e) 
    { 
        return e.Id == message.Id;
    });

    if (message.Status == RequestQuestUpdate::Started || 
        message.Status == RequestQuestUpdate::StageUpdate)
    {
        // in order to prevent bugs when a quest is in progress
        // and being updated we add it as a new quest record to
        // maintain a proper remote questlog state.
        if (questIt == entries.end())
        {
            auto& newQuest = entries.emplace_back();
            newQuest.Id = message.Id;
            newQuest.Stage = message.Stage;

            if (message.Status == RequestQuestUpdate::Started)
            {
                spdlog::info("Started Quest: {:x}:{}", message.Id.BaseId, message.Id.ModId);

                // we only trigger that on remote quest start
                const Script::Player scriptPlayer(*it, m_world);
                const Script::Quest scriptQuest(message.Id.BaseId, message.Stage, m_world);

                m_world.GetScriptService().HandleQuestStart(scriptPlayer, scriptQuest);
            }
        } 
        else 
        {
            spdlog::info("Updated quest: {:x}:{}", message.Id.BaseId, message.Stage);

            auto& record = *questIt;
            record.Id = message.Id;
            record.Stage = message.Stage;

            const Script::Player scriptPlayer(*it, m_world);
            const Script::Quest scriptQuest(message.Id.BaseId, message.Stage, m_world);

            m_world.GetScriptService().HandleQuestStage(scriptPlayer, scriptQuest);
        }
    }
    else if (message.Status == RequestQuestUpdate::Stopped)
    {
        spdlog::info("Stopped quest: {:x}", message.Id.BaseId);

        const Script::Player player(*it, m_world);
        m_world.GetScriptService().HandleQuestStop(player, message.Id.BaseId);

        if (questIt != entries.end())
        {
            entries.erase(questIt);
        }
        else
        {
            spdlog::warn("Unable to delete quest object {:x}", message.Id.BaseId);
        }
    }
}
  
// script wrapper
bool QuestService::StartStopQuest(entt::entity aRecipient, GameId aGameId, bool aStop) noexcept
{
    NotifyQuestUpdate questMsg;
    questMsg.Status = aStop ? NotifyQuestUpdate::Stopped : NotifyQuestUpdate::Started;
    questMsg.Id = aGameId;
    questMsg.Stage = 0;

    auto& playerComponent = m_world.get<PlayerComponent>(aRecipient);
    GameServer::Get()->Send(playerComponent.ConnectionId, questMsg);

    return true;
}
