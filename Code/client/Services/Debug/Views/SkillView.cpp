#include <Services/DebugService.h>

#include <imgui.h>

#include <PlayerCharacter.h>

// TODO: ft
void DebugService::DrawSkillView()
{
#if TP_SKYRIM64
    ImGui::Begin("Skills");

    PlayerCharacter* pPlayer = PlayerCharacter::Get();
    Skills* pSkills = *pPlayer->pSkills;

    ImGui::InputFloat("Global XP", &pSkills->xp, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat("Global level threshold", &pSkills->levelThreshold, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);

    for (int i = 0; i < Skills::kTotal; i++)
    {
        const char* skillString = Skills::GetSkillString((Skills::Skill)i);
        if (!ImGui::CollapsingHeader(skillString, ImGuiTreeNodeFlags_DefaultOpen))
            continue;

        Skills::SkillData pSkill = pSkills->skills[i];
        ImGui::InputFloat("Level", &pSkill.level, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("XP", &pSkill.xp, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Level threshold", &pSkill.levelThreshold, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);

        uint32_t legendaryLevel = pSkills->legendaryLevels[i];
        ImGui::InputScalar("Legendary level", ImGuiDataType_U32, (void*)&legendaryLevel, nullptr, nullptr, nullptr,
                           ImGuiInputTextFlags_ReadOnly);
    }

    ImGui::End();
#endif
}
