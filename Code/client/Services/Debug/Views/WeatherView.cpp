#include <Services/DebugService.h>

#include <imgui.h>
#include <inttypes.h>

#include <Sky/Sky.h>
#include <Forms/TESWeather.h>

void DebugService::DrawWeatherView()
{
    ImGui::Begin("Weather");

    Sky* pSky = Sky::Get();

    TESWeather* pCurrentWeather = pSky->GetWeather();
    if (pCurrentWeather)
    {
        ImGui::InputScalar("Current weather ID", ImGuiDataType_U32, &pCurrentWeather->formID, 0, 0, "%" PRIx32,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
    }

    ImGui::Separator();

    ImGui::InputScalar("Weather updates enabled?", ImGuiDataType_U8, &Sky::s_shouldUpdateWeather, 0, 0, "%" PRIx8,
                       ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Toggle weather updates"))
        Sky::s_shouldUpdateWeather = !Sky::s_shouldUpdateWeather;

    ImGui::Separator();

    static uint32_t s_weatherId = 0;
    ImGui::InputScalar("New weather ID", ImGuiDataType_U32, &s_weatherId, 0, 0, "%" PRIx32,
                       ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Set weather"))
    {
        TESWeather* pWeather = Cast<TESWeather>(TESForm::GetById(s_weatherId));
        if (pWeather)
            pSky->SetWeather(pWeather);
    }

    if (ImGui::Button("Force weather"))
    {
        TESWeather* pWeather = Cast<TESWeather>(TESForm::GetById(s_weatherId));
        if (pWeather)
            pSky->ForceWeather(pWeather);
    }

    ImGui::Separator();

    if (ImGui::Button("Reset weather"))
    {
        pSky->ResetWeather();
    }

    ImGui::End();
}
