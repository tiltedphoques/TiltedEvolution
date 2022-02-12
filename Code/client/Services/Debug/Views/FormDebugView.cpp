#include <imgui.h>
#include <inttypes.h>
#include <Games/TES.h>

#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>

#include <Services/TestService.h>

void TestService::DrawFormDebugView()
{
    static uint32_t formId = 0;
    static Actor* pActor = nullptr;

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, &formId, 0, 0, "%" PRIx32,
                       ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Look up"))
    {
        if (formId)
        {
            TESForm* pFetchForm = TESForm::GetById(formId);
            if (pFetchForm)
                pActor = RTTI_CAST(pFetchForm, TESForm, Actor);
        }
    }

    if (pActor)
    {
        ImGui::InputScalar("Memory address", ImGuiDataType_U64, (void*)&pActor, 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
    }
}
