#include <stdafx.h>

#include <World.h>
#include <Scripting/Quest.h>
#include <Components.h>

#include <Messages/NotifyQuestUpdate.h>

namespace Script
{
void Quest::SetStage(uint16_t aNewStage, std::vector<Player> aPlayers)
{
    // update the remote quest nodes
    size_t counter = 0;
    for (auto& player : aPlayers)
    {
        auto handle = player.GetEntityHandle();
        auto& component = m_pWorld->get<QuestLogComponent>(handle);
        auto& questList = component.QuestContent.Entries;

        auto it = std::find_if(questList.begin(), questList.end(), [this](auto& e) { return e.Id.BaseId == m_id; });

        if (it != questList.end())
        {
            it->Stage = aNewStage;
            counter++;

            player.UpdateQuest(*this);
        }
    }

    // update our node
    m_stage = aNewStage;
}
} // namespace Script
