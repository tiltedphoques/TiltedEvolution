#include <Services/DebugService.h>
#include <Services/QuestService.h>
#include <Services/PapyrusService.h>

#include <PlayerCharacter.h>
#include <Forms/TESQuest.h>

#include <imgui.h>

void DebugService::DrawQuestDebugView()
{
    auto* pPlayer = PlayerCharacter::Get();
    if (!pPlayer) return;

    ImGui::Begin("Quest log");

    for (auto &objective : pPlayer->objectives)
    {
        auto* pQuest = objective.instance->quest;
        if (!pQuest)
            continue;

        if (QuestService::IsNonSyncableQuest(pQuest))
            continue;

        if (pQuest->IsActive())
        {
            ImGui::TextColored({255.f, 0.f, 255.f, 255.f}, "%s|%x|%s", pQuest->idName.AsAscii(),
                               pQuest->flags, pQuest->fullName.value.AsAscii());

            for (auto* stage : pQuest->stages)
            {
                ImGui::TextColored({0.f, 255.f, 255.f, 255.f}, "Stage: %d|%x", stage->stageIndex, stage->flags);

                if (ImGui::Button("Set stage"))
                    pQuest->ScriptSetStage(stage->stageIndex);
            }
        }
        else
        {
            ImGui::Text("%s|%x|%s", pQuest->idName.AsAscii(), pQuest->flags, pQuest->fullName.value.AsAscii());
        }
    }

    ImGui::End();
}
