
#include <Games/Skyrim/Shared/Sky/Sky.h>
#include <Services/TestService.h>
#include <imgui.h>

// NOTE(Force): It looks like we can only apply new weathers that are present in TESWeatherList, however
// we might be able to add our own at runtime?

static void DrawWeatherInfo(const char* aWeatherName, TESWeather* apWeather)
{
    if (ImGui::CollapsingHeader(aWeatherName, ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Name %s\nFormID %x", apWeather->GetFormEditorID(), apWeather->formID);
        ImGui::Text("Aurora Name: %s", apWeather->aurora.name.AsAscii());
        const auto flags = apWeather->data.flags;
        if (flags & TESWeather::WeatherDataFlag::kSnow)
            ImGui::Text("Is Snowy");
        if (flags & TESWeather::WeatherDataFlag::kRainy)
            ImGui::Text("Is Rainy");
        if (flags & TESWeather::WeatherDataFlag::kPermAurora)
            ImGui::Text("Is PermAurora");
        if (flags & TESWeather::WeatherDataFlag::kCloudy)
            ImGui::Text("Is Cloudy");
        if (flags & TESWeather::WeatherDataFlag::kAuroraFollowsSun)
            ImGui::Text("Is AuroraFollowsSun");
        if (flags & TESWeather::WeatherDataFlag::kPleasant)
            ImGui::Text("Is Pleasant");

        if (ImGui::Button("Apply"))
            Sky::GetInstance()->SetWeatherExternal(apWeather, true, true);
    }
}

static void DrawClimateInfo(TESClimate* apClimate)
{
    if (ImGui::CollapsingHeader("Current Climate", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Name %s\nFormID %x", apClimate->GetFormEditorID(), apClimate->formID);
        ImGui::Text("Daylight object %s\nNighttime object %s", apClimate->txSkyObjects[0].name.AsAscii(),
                    apClimate->txSkyObjects[1].name.AsAscii());

        if (ImGui::CollapsingHeader("Timing Data", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Sunrise: %d, %d", apClimate->timing.sunrise.begin, apClimate->timing.sunrise.end);
            ImGui::Text("SunSet: %d, %d", apClimate->timing.sunset.begin, apClimate->timing.sunset.end);
            ImGui::Text("Volatility: %d", apClimate->timing.volatility);
            ImGui::Text("MoonPhaseLength: %d", apClimate->timing.moonPhaseLength);
        }

        ImGui::Text("Used weathers:");
        int i = 0;
        for (auto item : apClimate->weatherList)
        {
            char buf[8]{};
            sprintf_s(buf, 8, "%d", i);
            DrawWeatherInfo(buf, item->pWeather);
            ImGui::Text("Chance %d", item->uiChance);
            i++;
        }
    }
}

void TestService::DrawSkyDebugView()
{
    auto* pInstance = Sky::GetInstance();
    if (!pInstance)
        return;

    if (TESClimate* pClimate = pInstance->GetCurrentClimate())
    {
        DrawClimateInfo(pClimate);
    }

    ImGui::Separator();

    if (TESWeather* pWeather = pInstance->GetCurrentWeather())
        DrawWeatherInfo("CurrentWeather", pWeather);

    if (TESWeather* pWeather = pInstance->GetLastWeather())
        DrawWeatherInfo("LastWeather", pWeather);

    if (TESWeather* pWeather = pInstance->GetDefaultWeather())
        DrawWeatherInfo("DefaultWeather", pWeather);
}
