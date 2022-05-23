
#include <Forms/TESObject.h>
#include <Games/Skyrim/Interface/UI.h>
#include <Games/Skyrim/PlayerCharacter.h>
#include <Services/DebugService.h>
#include <imgui.h>

static void (*ConstructThisShit)(TESObjectREFR*);
static void (*GetHandle_TESREFR)(TESObjectREFR*, uint32_t*);

static void SpawnMapmarker()
{
    TESObjectREFR* refr = Memory::Allocate<TESObjectREFR>();
    ConstructThisShit(refr);
    // dont save this form
    refr->SetTemporary();

    refr->rotation = {};
    refr->position = PlayerCharacter::Get()->position;

    MapMarkerData* pMarkerData = MapMarkerData::New();
    // pMarkerData->name.value.Set(pActor->baseForm->GetName());
    pMarkerData->name.value.Set("Labradoodle");
    pMarkerData->cOriginalFlags = pMarkerData->cFlags = MapMarkerData::Flag::VISIBLE;
    pMarkerData->sType = MapMarkerData::Type::kCity;
    refr->extraData.SetMarkerData(pMarkerData);

    uint32_t handle = 0;
    GetHandle_TESREFR(refr, &handle);
    PlayerCharacter::Get()->CurrentMapmarkerRefHandles.Resize(
        PlayerCharacter::Get()->CurrentMapmarkerRefHandles.length + 1);
    PlayerCharacter::Get()->CurrentMapmarkerRefHandles[PlayerCharacter::Get()->CurrentMapmarkerRefHandles.length - 1] =
        handle;
}

static TiltedPhoques::Initializer s_initUiDebugView([]() {
    ConstructThisShit = reinterpret_cast<decltype(ConstructThisShit)>(0x140295760);
    GetHandle_TESREFR = reinterpret_cast<decltype(GetHandle_TESREFR)>(0x1402ADF90);
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
