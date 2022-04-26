#include <imgui.h>
#include <inttypes.h>
#include <Games/TES.h>

#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>

#include <Services/DebugService.h>
#include <AI/AIProcess.h>
#include <Misc/MiddleProcess.h>
#include <Effects/ActiveEffect.h>

void DebugService::DrawFormDebugView()
{
    static TESObjectREFR* pRefr = nullptr;
    static TESForm* pFetchForm = nullptr;

    ImGui::Begin("Form");

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, &m_formId, 0, 0, "%" PRIx32,
                       ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Look up"))
    {
        if (m_formId)
        {
            pFetchForm = TESForm::GetById(m_formId);
            if (pFetchForm)
                pRefr = Cast<TESObjectREFR>(pFetchForm);
        }
    }

    if (pFetchForm)
    {
        ImGui::InputScalar("Memory address", ImGuiDataType_U64, (void*)&pFetchForm, 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
    }

    if (pRefr)
    {
        char name[256];
        sprintf_s(name, std::size(name), "%s (%x)", pRefr->baseForm->GetName(), pRefr->formID);
        ImGui::InputText("Name", name, std::size(name), ImGuiInputTextFlags_ReadOnly);

        /*
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
        */
    }

    ImGui::End();
}
