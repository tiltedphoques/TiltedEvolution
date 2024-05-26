#include <Services/DebugService.h>

#include <imgui.h>
#include <inttypes.h>

#include <TimeManager.h>

void DebugService::DrawCalendarView()
{
    ImGui::Begin("Calendar");

    auto* pGameTime = TimeData::Get();

    auto year = pGameTime->GameYear->f;
    ImGui::InputFloat("Year", &year, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);

    auto month = pGameTime->GameMonth->f;
    ImGui::InputFloat("Month", &month, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);

    auto day = pGameTime->GameDay->f;
    ImGui::InputFloat("Day", &day, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);

    auto hour = pGameTime->GameHour->f;
    ImGui::InputFloat("Hour", &hour, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);

    auto passed = pGameTime->GameDaysPassed->f;
    ImGui::InputFloat("Passed", &passed, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);

    auto scale = pGameTime->TimeScale->f;
    ImGui::InputFloat("Scale", &scale, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);

    ImGui::End();
}
