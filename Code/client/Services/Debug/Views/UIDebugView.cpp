
#include <Forms/TESObject.h>
#include <Games/Skyrim/Interface/UI.h>
#include <Games/Skyrim/PlayerCharacter.h>
#include <Services/DebugService.h>
#include <imgui.h>

static void SpawnOurMapmarker(TESObjectREFR* apRefr, const char* apName, const MapMarkerData::Type aType)
{
    MapMarkerData* pMarkerData = MapMarkerData::New();
    pMarkerData->name.value.Set(apName);
    pMarkerData->cOriginalFlags = pMarkerData->cFlags = MapMarkerData::Flag::VISIBLE;
    pMarkerData->sType = aType; // "custom destination" marker either 66 or 0
    apRefr->extraData.SetMarkerData(pMarkerData);
    // hmm..
    uint32_t handle = 0;
    apRefr->GetHandle(handle);

    auto* pPlayer = PlayerCharacter::Get();
    pPlayer->CurrentMapmarkerRefHandles.Add(&handle);
}

static void SpawnMapmarker(const char* apName, float posoff)
{
    TESObjectREFR* pRefr = TESObjectREFR::New();
    pRefr->SetTemporary();

    // pos will later be fetched through TESObjectREFR::GetLookingAtLocation for scaleform
    pRefr->rotation = {};
    pRefr->position = PlayerCharacter::Get()->position;
    pRefr->position.x += posoff;

    SpawnOurMapmarker(pRefr, apName, MapMarkerData::Type::kMousePointer);
}

static void SpawnMapmarker2(const char* apName, float posoff, int i)
{
    TESObjectREFR* pRefr = TESObjectREFR::New();
    pRefr->SetTemporary();

    pRefr->rotation = {};
    pRefr->position = PlayerCharacter::Get()->position;
    pRefr->position.x += posoff;

    SpawnOurMapmarker(pRefr, apName, (MapMarkerData::Type)i);
}

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

    if (ImGui::Button("Place all test markers"))
    {
        float off = 0.f;
        for (int i = 0; i < 66; i++)
        {
            char buf[512];
            snprintf(buf, 512, "m%d", i);
            SpawnMapmarker2(buf, off, i);

            off += 2000.f;
        }

        // SpawnMapmarker("TEST");
    }
}
