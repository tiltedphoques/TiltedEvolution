
#include <Games/Skyrim/Shared/Sky/Sky.h>
#include <Services/TestService.h>
#include <imgui.h>

#include <Shared/TESForms/World/TESRegion.h>

// NOTE(Force): It looks like we can only apply new weathers that are present in TESWeatherList, however
// we might be able to add our own at runtime?

/*
On game start, the game goes through the TESDataHandler region array and sets all Weathers to WorldMapWeather.
Afterwards PlayerRegionState::LoadGame is called, to set the current weather from the savegame.

When changing a cell, all regions once again get updated in the TESDataHandler array.

Sky::Update
the game may decide to update the weather randomly.
and update all cell regions again.

when going inside a cell such as bleakfalls barrow, the game calls TESObjectCELL::UpdateManagedNodes
and sets all weathers to null, to disable weather processing.
*/

// TESObjectCell::GetRegionList()

struct PlayerRegionState
{
    char pad0[72];
    TESRegion* pLastKnownWeatherRegion;

    static PlayerRegionState* Get()
    {
        return (PlayerRegionState*)0x141F5E388;
    }
};

static void DrawWeatherInfo2(TESWeather* apWeather)
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

static void DrawWeatherHeader(const char *apName, TESWeather* apWeather)
{
    if (ImGui::CollapsingHeader(apName))
    {
        DrawWeatherInfo2(apWeather);
    }
}

static void DrawClimateInfo(TESClimate* apClimate)
{
    if (ImGui::CollapsingHeader("Current Climate"))
    {
        ImGui::Indent(10.f);
        ImGui::Text("Name %s\nFormID %x", apClimate->GetFormEditorID(), apClimate->formID);
        ImGui::Text("Daylight object %s\nNighttime object %s", apClimate->txSkyObjects[0].name.AsAscii(),
                    apClimate->txSkyObjects[1].name.AsAscii());

        if (ImGui::TreeNode("Timing Data"))
        {
            ImGui::Text("Sunrise: %d, %d", apClimate->timing.sunrise.begin, apClimate->timing.sunrise.end);
            ImGui::Text("SunSet: %d, %d", apClimate->timing.sunset.begin, apClimate->timing.sunset.end);
            ImGui::Text("Volatility: %d", apClimate->timing.volatility);
            ImGui::Text("MoonPhaseLength: %d", apClimate->timing.moonPhaseLength);
            ImGui::TreePop();
        }

        ImGui::Text("Used weathers:");
        int i = 0;
        for (auto item : apClimate->weatherList)
        {
            char buf[8]{};
            sprintf_s(buf, 8, "%d", i);
            if (ImGui::TreeNode(buf))
            {
                DrawWeatherInfo2(item->pWeather);
                ImGui::TreePop();
            }
            ImGui::Text("Chance %d", item->uiChance);
            i++;
        }
        ImGui::Indent(0.f);
    }
}

static void DrawRegionInfo(const char* acRegionName, TESRegion* apRegion)
{
    if (ImGui::CollapsingHeader(acRegionName, ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Name %s\nFormID %x", apRegion->GetFormEditorID(), apRegion->formID);

        static TESWeather* pCurrentWeather{nullptr};
        if (ImGui::Button("New Random Weather"))
        {
            pCurrentWeather = apRegion->SelectWeather();
        }

        if (pCurrentWeather)
            DrawWeatherHeader("Current Random Weather", pCurrentWeather);
    }
}

void TestService::DrawSkyDebugView()
{
    auto* pInstance = Sky::GetInstance();
    if (!pInstance)
        return;

    ImGui::Text("CurrentGameHour: %f\nLastWeatherUpdate: %f\nCurrentWeatherPercent: %f\nCurrentWeatherDuration: %f",
                pInstance->GetCurrentGameHour(), pInstance->GetLastWeatherUpdate(),
                pInstance->GetCurrentWeatherPercent(), pInstance->GetCurrentWeatherDuration());
    ImGui::Text("GetCurrentWeatherTimeElapsed: %f", pInstance->GetCurrentWeatherTimeElapsed());

    if (TESClimate* pClimate = pInstance->GetCurrentClimate())
        DrawClimateInfo(pClimate);

    ImGui::Spacing();

    static ImVec4 kRedColor = ImVec4(255.f, 0.f, 0.f, 255.f);
    if (TESWeather* pWeather = pInstance->GetCurrentWeather())
        DrawWeatherHeader("CurrentWeather", pWeather);
    else
        ImGui::TextColored(kRedColor, "No CurrentWeather");
    if (TESWeather* pWeather = pInstance->GetLastWeather())
        DrawWeatherHeader("LastWeather", pWeather);
    else
        ImGui::TextColored(kRedColor, "No LastWeather");
    if (TESWeather* pWeather = pInstance->GetOverrideWeather())
        DrawWeatherHeader("OverrideWeather", pWeather);
    else
        ImGui::TextColored(kRedColor, "No OverrideWeather");
    if (TESWeather* pWeather = pInstance->GetDefaultWeather())
        DrawWeatherHeader("DefaultWeather", pWeather);
    ImGui::Separator();
    if (TESRegion* pRegion = pInstance->GetCurrentRegion())
        DrawRegionInfo("Current Sky Region", pRegion);
    ImGui::Separator();
    // NOTE(Force): By setting pLastKnownWeatherRegion the game seems to update the region
    if (TESRegion* pRegion = PlayerRegionState::Get()->pLastKnownWeatherRegion)
        DrawRegionInfo("LastKnownWeatherRegion", pRegion);

    // SetCurrentWeather(TESRegion *this, TESWeather *apWeather)
}
