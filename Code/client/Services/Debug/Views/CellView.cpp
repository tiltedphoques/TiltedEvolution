#include <Services/DebugService.h>

#include <imgui.h>
#include <inttypes.h>
#include <Games/TES.h>

#include <PlayerCharacter.h>

#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>

void DebugService::DrawCellView()
{
    ImGui::Begin("Cell");

    PlayerCharacter* pPlayer = PlayerCharacter::Get();

    if (auto* pWorldSpace = pPlayer->GetWorldSpace())
    {
        if (ImGui::CollapsingHeader("World space", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const uint32_t worldFormId = pWorldSpace->formID;
            ImGui::InputScalar("Id", ImGuiDataType_U32, (void*)&worldFormId, nullptr, nullptr, "%" PRIx32,
                               ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

            char* pName = (char*)pWorldSpace->GetName();
            size_t nameLen = strlen(pName);
            ImGui::InputText("Name", pName, nameLen, ImGuiInputTextFlags_ReadOnly);

            // TODO: ft
        #if TP_SKYRIM64
            char* pEditorId = (char*)pWorldSpace->GetFormEditorID();
            size_t editorIdLen = strlen(pEditorId);
            ImGui::InputText("Editor ID", pEditorId, editorIdLen, ImGuiInputTextFlags_ReadOnly);
        #endif
        }
    }

    if (auto* pCell = pPlayer->parentCell)
    {
        if (ImGui::CollapsingHeader("Parent cell", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const uint32_t cellId = pCell->formID;
            ImGui::InputScalar("Id", ImGuiDataType_U32, (void*)&cellId, nullptr, nullptr, "%" PRIx32,
                               ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

            char* pName = (char*)pCell->GetName();
            size_t nameLen = strlen(pName);
            ImGui::InputText("Name", pName, nameLen, ImGuiInputTextFlags_ReadOnly);

            // TODO: ft
        #if TP_SKYRIM64
            char* pEditorId = (char*)pCell->GetFormEditorID();
            size_t editorIdLen = strlen(pEditorId);
            ImGui::InputText("Editor ID", pEditorId, editorIdLen, ImGuiInputTextFlags_ReadOnly);
        #endif
        }
    }

    ImGui::End();
}
