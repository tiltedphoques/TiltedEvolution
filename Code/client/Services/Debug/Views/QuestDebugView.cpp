#include <Services/DebugService.h>
#include <Services/QuestService.h>
#include <Services/PapyrusService.h>
#include <Services/PartyService.h>

#include <PlayerCharacter.h>
#include <Forms/TESQuest.h>
#include <Forms/TESObjectCELL.h>

#include <Events/MoveActorEvent.h>

#include <imgui.h>

// TODO: ft
void DebugService::DrawQuestDebugView()
{
#if TP_SKYRIM64
    auto* pPlayer = PlayerCharacter::Get();
    if (!pPlayer) return;

    ImGui::SetNextWindowSize(ImVec2(250, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin("Quests");

    // TODO(cosideci): yes I'll refactor this
    if (!m_world.GetPartyService().IsLeader())
    {
        ImGui::Text("You need to be the quest leader to access the quest debugger.");
    }
    else
    {
        Set<uint32_t> foundQuests{};

        for (auto &objective : pPlayer->objectives)
        {
            TESQuest* pQuest = objective.instance->quest;
            if (!pQuest)
                continue;

            if (QuestService::IsNonSyncableQuest(pQuest))
                continue;

            if (foundQuests.contains(pQuest->formID))
                continue;

            foundQuests.insert(pQuest->formID);

            if (!pQuest->IsActive())
                continue;

            char questName[256];
            sprintf_s(questName, std::size(questName), "%s (%s)", pQuest->fullName.value.AsAscii(), pQuest->idName.AsAscii());
            if (!ImGui::CollapsingHeader(questName))
                continue;

            if (ImGui::CollapsingHeader("Stages"))
            {
                for (auto* pStage : pQuest->stages)
                {
                    ImGui::TextColored({0.f, 255.f, 255.f, 255.f}, "Stage: %d, is done? %s", pStage->stageIndex, pStage->IsDone() ? "true" : "false");

                    char setStage[64];
                    sprintf_s(setStage, std::size(setStage), "Set stage (%d)", pStage->stageIndex);

                    if (ImGui::Button(setStage))
                        pQuest->ScriptSetStage(pStage->stageIndex);
                }
            }

            if (ImGui::CollapsingHeader("Actors"))
            {
                Set<uint32_t> foundActors{};

                for (BGSScene* pScene : pQuest->scenes)
                {
                    for (uint32_t actorId : pScene->actorIds)
                    {
                        Actor* pActor = Cast<Actor>(pQuest->GetAliasedRef(actorId));
                        if (!pActor)
                            continue;

                        if (foundActors.contains(pActor->formID))
                            continue;

                        foundActors.insert(pActor->formID);

                        char name[256];
                        sprintf_s(name, std::size(name), "%s (%x)", pActor->baseForm->GetName(), pActor->formID);
                        ImGui::BulletText(name);

                        ImGui::PushID(pActor->formID);
                        if (ImGui::Button("Teleport to me"))
                        {
                            auto* pPlayer = PlayerCharacter::Get();
                            m_world.GetRunner().Trigger(MoveActorEvent(pActor->formID, pPlayer->parentCell->formID, pPlayer->position));
                        }
                        ImGui::PopID();
                    }
                }
            }
        }
    }

    ImGui::End();
#endif
}
