#include <imgui.h>
#include <inttypes.h>
#include <Games/TES.h>

#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>

#include <Services/TestService.h>
#include <AI/AIProcess.h>
#include <Misc/MiddleProcess.h>
#include <Effects/ActiveEffect.h>

void TestService::DrawFormDebugView()
{
    static Actor* pActor = nullptr;

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, &m_formId, 0, 0, "%" PRIx32,
                       ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Look up"))
    {
        if (m_formId)
        {
            TESForm* pFetchForm = TESForm::GetById(m_formId);
            if (pFetchForm)
                pActor = RTTI_CAST(pFetchForm, TESForm, Actor);
        }
    }

    if (pActor)
    {
        ImGui::InputScalar("Memory address", ImGuiDataType_U64, (void*)&pActor, 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);

        for (ActiveEffect* pEffect : *pActor->currentProcess->middleProcess->ActiveEffects)
        {
            if (!pEffect)
                continue;

            if (!ImGui::CollapsingHeader(pEffect->pSpell->fullName.value, ImGuiTreeNodeFlags_DefaultOpen))
                continue;

            ImGui::InputFloat("Elapsed seconds", &pEffect->fElapsedSeconds, 0, 0, "%.1f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Duration", &pEffect->fDuration, 0, 0, "%.1f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Magnitude", &pEffect->fMagnitude, 0, 0, "%.1f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputInt("Flags", (int*)&pEffect->uiFlags, 0, 0, ImGuiInputTextFlags_ReadOnly);
        }
    }
}
