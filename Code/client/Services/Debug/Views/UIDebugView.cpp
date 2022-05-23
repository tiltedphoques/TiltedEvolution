
#include <imgui.h>
#include <Games/Skyrim/Interface/UI.h>
#include <Games/Skyrim/PlayerCharacter.h>
#include <Services/DebugService.h>
#include <Forms/TESObject.h>

static void (*ConstructThisShit)(TESObjectREFR*);

static void SpawnMapmarker()
{
    TESObjectREFR* refr = Memory::Allocate<TESObjectREFR>();
    ConstructThisShit(refr);
    // dont save this form
    refr->SetTemporary();

    refr->rotation = {};
    refr->position = PlayerCharacter::Get()->position;

    MapMarkerData* pMarkerData = MapMarkerData::New();
    //pMarkerData->name.value.Set(pActor->baseForm->GetName());
    pMarkerData->name.value.Set("Labradoodle");
    pMarkerData->flags = MapMarkerData::Flag::VISIBLE;
    pMarkerData->type = MapMarkerData::Type::kGiantCamp;
    refr->extraData.SetMarkerData(pMarkerData);
}

static TiltedPhoques::Initializer s_initUiDebugView([]() {
    ConstructThisShit = reinterpret_cast<decltype(ConstructThisShit)>(0x140295760);
});

void DebugService::DrawUIView()
{
    ImGui::MenuItem("Show build tag", nullptr, &m_showBuildTag);
    if (ImGui::Button("Log all open windows"))
    {
        UI* pUI = UI::Get();
        for (const auto& it : pUI->menuMap)
        {
            if (pUI->GetMenuOpen(it.key))
                spdlog::info("{}", it.key.AsAscii());
        }
    }

    if (ImGui::Button("Close all menus"))
    {
        UI::Get()->CloseAllMenus();
    }

    if (ImGui::Button("Place test mapmarker"))
    {
        SpawnMapmarker();
    }
}
