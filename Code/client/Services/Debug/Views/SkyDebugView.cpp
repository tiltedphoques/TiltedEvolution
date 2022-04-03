
#include <Games/Skyrim/Shared/Sky/Sky.h>
#include <Services/TestService.h>
#include <imgui.h>

static void DrawWeatherInfo(const char *aWeatherName, TESWeather* apWeather)
{
    if (ImGui::CollapsingHeader(aWeatherName, ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Name %s\nFormID %d", apWeather->GetFormEditorID(), apWeather->formID);
        ImGui::Text("Aurora Name: %s", apWeather->aurora.name.AsAscii());
        auto flag = apWeather->data.flags;
        if (flag & TESWeather::WeatherDataFlag::kSnow)
            ImGui::Text("Is Snowy");
        if (flag & TESWeather::WeatherDataFlag::kRainy)
            ImGui::Text("Is Rainy");
        if (flag & TESWeather::WeatherDataFlag::kPermAurora)
            ImGui::Text("Is PermAurora");
        if (flag & TESWeather::WeatherDataFlag::kCloudy)
            ImGui::Text("Is Cloudy");
        if (flag & TESWeather::WeatherDataFlag::kAuroraFollowsSun)
            ImGui::Text("Is AuroraFollowsSun");
        if (flag & TESWeather::WeatherDataFlag::kPleasant)
            ImGui::Text("Is Pleasant");
    }
}

void TestService::DrawSkyDebugView()
{
    auto* pInstance = Sky::GetInstance();
    if (!pInstance)
        return;

    if (TESClimate* pClimate = pInstance->GetCurrentClimate())
    {
        if (ImGui::CollapsingHeader("Current Climate", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Daylight object %s\nNighttime object %s",
                        pClimate->txSkyObjects[0].name.AsAscii(), pClimate->txSkyObjects[1].name.AsAscii());

            ImGui::Text("Used weathers:");
            for (auto item : pClimate->weatherList)
            {
                DrawWeatherInfo("", item->pWeather);
                ImGui::Text("Chance %d", item->uiChance);
            }

            ImGui::Text("Timing:");

            ImGui::Text("Sunrise: %d, %d", pClimate->timing.sunrise.begin, pClimate->timing.sunrise.end);
            ImGui::Text("SunSet: %d, %d", pClimate->timing.sunset.begin, pClimate->timing.sunset.end);
            ImGui::Text("Volatility: %d", pClimate->timing.volatility);
            ImGui::Text("MoonPhaseLength: %d", pClimate->timing.moonPhaseLength);
        }
    }

    if (TESWeather* pWeather = pInstance->GetCurrentWeather())
    {
        DrawWeatherInfo("CurrentWeather", pWeather);
    }

    if (ImGui::Button("Set Weather"))
    {
    }
}
