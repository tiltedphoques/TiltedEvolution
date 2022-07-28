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
    static TESForm* pFetchForm = nullptr;
    static TESObjectREFR* pRefr = nullptr;

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
        if (auto* pParentCell = pRefr->GetParentCell())
        {
            const uint32_t cellId = pParentCell->formID;
            ImGui::InputScalar("GetParentCell", ImGuiDataType_U32, (void*)&cellId, nullptr, nullptr, "%" PRIx32,
                               ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        }

        if (auto* pParentCell = pRefr->parentCell)
        {
            const uint32_t cellId = pParentCell->formID;
            ImGui::InputScalar("parentCell", ImGuiDataType_U32, (void*)&cellId, nullptr, nullptr, "%" PRIx32,
                               ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        }

        /*
        char name[256];
        sprintf_s(name, std::size(name), "%s (%x)", pRefr->baseForm->GetName(), pRefr->formID);
        ImGui::InputText("Name", name, std::size(name), ImGuiInputTextFlags_ReadOnly);

        for (ActiveEffect* pEffect : *pRefr->currentProcess->middleProcess->ActiveEffects)
        {
            if (!pEffect)
                continue;

            if (!ImGui::CollapsingHeader(pEffect->pSpell->fullName.value, ImGuiTreeNodeFlags_DefaultOpen))
                continue;

            ImGui::InputFloat("Elapsed seconds", &pEffect->fElapsedSeconds, 0, 0, "%.1f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Duration", &pEffect->fDuration, 0, 0, "%.1f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Magnitude", &pEffect->fMagnitude, 0, 0, "%.1f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputInt("Flags", (int*)&pEffect->uiFlags, 0, 0, ImGuiInputTextFlags_ReadOnly);

            if (ImGui::Button("Elapse time"))
                m_world.GetRunner().Queue([pEffect]() { pEffect->fElapsedSeconds = pEffect->fDuration - 3.f; });
        }
        */
    }

    ImGui::End();
}
