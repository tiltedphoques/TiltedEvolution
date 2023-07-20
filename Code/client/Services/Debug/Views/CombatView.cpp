#include <Services/DebugService.h>
#include <imgui.h>
#include <inttypes.h>

#if TP_SKYRIM64
#include <Games/Combat/CombatController.h>
#include <Combat/CombatGroup.h>
#endif

void DebugService::DrawCombatView()
{
#if TP_SKYRIM64
    if (!m_formId)
        return;

    ImGui::Begin("Combat");

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, (void*)&m_formId, nullptr, nullptr, "%" PRIx32, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

    Actor* pActor = Cast<Actor>(TESForm::GetById(m_formId));

    if (pActor && pActor->pCombatController && pActor->pCombatController->pCombatGroup)
    {
        ImGui::BeginChild("Targets", ImVec2(0, 200), true);

        auto& targets = pActor->pCombatController->pCombatGroup->targets;
        for (int i = 0; i < targets.length; i++)
        {
            auto& target = targets[i];
            Actor* pTarget = Cast<Actor>(TESObjectREFR::GetByHandle(target.targetHandle));
            if (pTarget)
            {
                const uint32_t formID = pTarget->formID;
                ImGui::InputScalar("Form ID", ImGuiDataType_U32, (void*)&formID, nullptr, nullptr, "%" PRIx32, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
            }

            ImGui::Separator();
        }

        ImGui::EndChild();
    }

    ImGui::End();
#endif
}
