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

        if (pQuest->IsActive())
        {
            ImGui::TextColored({255.f, 0.f, 255.f, 255.f}, "%s|%x|%s", pQuest->idName.AsAscii(),
                               pQuest->flags, pQuest->fullName.value.AsAscii());

            for (auto* stage : pQuest->stages)
            {
                ImGui::TextColored({0.f, 255.f, 255.f, 255.f}, "Stage: %d, is done? %s", stage->stageIndex, stage->IsDone() ? "true" : "false");

                char setStage[64];
                sprintf_s(setStage, std::size(setStage), "Set stage (%d)", stage->stageIndex);

                if (ImGui::Button(setStage))
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
