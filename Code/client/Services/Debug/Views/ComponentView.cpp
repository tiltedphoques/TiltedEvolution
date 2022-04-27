
#include <Components.h>
#include <World.h>
#include <imgui.h>
#include <inttypes.h>
#include <services/DebugService.h>

#if (TP_SKYRIM64)
#include <BSGraphics/BSGraphicsRenderer.h>
#include <Camera/PlayerCamera.h>
#include <Games/Skyrim/Forms/TESForm.h>
#include <Games/Skyrim/Interface/Menus/HUDMenuUtils.h>
#include <Games/Skyrim/NetImmerse/NiCamera.h>
#include <Games/Skyrim/TESObjectREFR.h>
#include <NetImmerse/NiCamera.h>
#else
#include <Games/Fallout4/Forms/TESForm.h>
#endif

#if (TP_SKYRIM64)

namespace
{
#if 0
constexpr float fFloatQuestMarkerMaxDistance = 2000.f;
constexpr float fFloatQuestMarkerMinDistance = 1000.f;

float CalculateFloatingQuestMarkerAlpha()
{
    float v1 = fsqrt((__m128)LODWORD(this->fDistanceToPlayerSqr)).m128_f32[0] -
                         fFloatQuestMarkerMaxDistance) /
                         (fFloatQuestMarkerMinDistance - fFloatQuestMarkerMaxDistance)) * 100.0;
    if (v1 > 100.0)
        return FLOAT_100_0;
    result = 0.0;
    if (v1 >= 0.0)
        return v1;
    return result;
}
#endif

static __declspec(noinline) bool DrawInWorldSpace(TESObjectREFR* apRefr, ImVec2& outViewPos)
{
    // Attach at the head ish.
    auto pos = apRefr->position;
    pos.z -= apRefr->GetHeight();

    NiPoint3 screenPoint{};
    HUDMenuUtils::WorldPtToScreenPt3(pos, screenPoint);
    // Calculate window collision bounds.
    auto* pViewport = BSGraphics::GetMainWindow();
    const NiRect<float> bounds = {
        static_cast<float>(pViewport->iWindowX),
        static_cast<float>(pViewport->iWindowX + pViewport->uiWindowWidth),
        static_cast<float>(pViewport->iWindowY),
        static_cast<float>(pViewport->iWindowY + pViewport->uiWindowHeight),
    };

    // translate to screen
    const ImVec2 screenPos = ImVec2{
        (pViewport->uiWindowWidth * screenPoint.x) + bounds.left,
        (pViewport->uiWindowHeight * (1.0f - screenPoint.y)) + bounds.top,
    };

    // implements HUDMarkerData::CalculateFloatingMarkerPositionAndVisiblity from FO4
    auto IsVisible = [](const ImVec2& aVec2, const NiRect<float>& aScreenBounds, const float zCoordGame) {
        if (aVec2.x >= aScreenBounds.left && aVec2.x <= aScreenBounds.right)
        {
            if (aVec2.y >= aScreenBounds.top &&
                aVec2.y <= aScreenBounds.bottom
                /*Not too sure about the Z coord check*/
                && zCoordGame >= 0)
                return true;
        }
        return false;
    };

    // Obviously it would be much smarter to attach the player name tag to the head bone,
    // because the player animation also influences the pos for example when jumping,
    // but the bounding box is fixed.
    // and offset it slightly above.
    // But this is just a demo...
    if (IsVisible(screenPos, bounds, screenPoint.z))
    {
        outViewPos = screenPos;
        return true;
    }

    outViewPos = {};
    return false;
}
} // namespace

// TODO(Force): Net Histrogram (waves)
// Engine stuff.
// Fix cursor.

static TESForm* g_SelectedForm{nullptr};

void DebugService::DrawComponentDebugView()
{
    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Component view", &m_toggleComponentWindow);

    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    Vector<entt::entity> entities(remoteView.begin(), remoteView.end());

    static uint32_t s_selectedRemoteId = 0;
    static uint32_t s_selectedRemote = 0;
    static entt::entity s_selectedEnt{};

    if (ImGui::Button("Reset Selection"))
    {
        g_SelectedForm = nullptr;
    }

    int i = 0;
    for (auto entity : entities)
    {
        auto& remoteComponent = remoteView.get<RemoteComponent>(entity);
        auto& formComponent = remoteView.get<FormIdComponent>(entity);

        char buffer[32];
        if (ImGui::Selectable(itoa(remoteComponent.Id, buffer, 16), s_selectedRemoteId == remoteComponent.Id))
        {
            s_selectedRemoteId = remoteComponent.Id;
            g_SelectedForm = TESForm::GetById(formComponent.Id);
            s_selectedEnt = entities[s_selectedRemote];
        }

        if (s_selectedRemoteId == remoteComponent.Id)
            s_selectedRemote = i;

        ++i;
    }
    ImGui::End();

    if (m_drawComponentsInWorldSpace && g_SelectedForm)
    {
        if (auto* pObject = Cast<TESObjectREFR>(g_SelectedForm))
        {
            ImVec2 screenPos{};
            if (DrawInWorldSpace(pObject, screenPos))
            {
                ImGui::SetNextWindowPos(screenPos);
                ImGui::Begin("Component debug");

                auto& remote = remoteView.get<RemoteComponent>(s_selectedEnt);
                ImGui::InputScalar("Server ID", ImGuiDataType_U32, &remote.Id, 0, 0, "%" PRIx32,
                                   ImGuiInputTextFlags_CharsHexadecimal);
                ImGui::InputScalar("Cached ref ID", ImGuiDataType_U32, &remote.CachedRefId, 0, 0, "%" PRIx32,
                                   ImGuiInputTextFlags_CharsHexadecimal);

                if (ImGui::CollapsingHeader("InterpolationComponent"))
                {
                    if (auto* pComponent = m_world.try_get<InterpolationComponent>(s_selectedEnt))
                    {
                        ImGui::Text("%f,%f,%f\n", pComponent->Position.x, pComponent->Position.y,
                                    pComponent->Position.z);
                    }
                }

                if (ImGui::CollapsingHeader("FaceGenComponent"))
                {
                    if (auto* pComponent = m_world.try_get<FaceGenComponent>(s_selectedEnt))
                    {
                        for (auto x : pComponent->FaceTints.Entries)
                        {
                            ImGui::Text("Alpha %f, Color %u, Type %u\n", x.Alpha, x.Color, x.Type);
                        }
                    }
                }

                if (ImGui::CollapsingHeader("RemoteAnimationComponent"))
                {
                    if (auto* pComponent = m_world.try_get<RemoteAnimationComponent>(s_selectedEnt))
                    {
                        ImGui::Text("EventName: %s\nTargetEventName: %s\nState1: %u\nState2: %u",
                                    pComponent->LastRanAction.EventName.c_str(),
                                    pComponent->LastRanAction.TargetEventName.c_str(),
                                    pComponent->LastRanAction.State1,
                                    pComponent->LastRanAction.State2);
                    }
                    // ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 30.f, screenPos,
                    //                                         ImColor::ImColor(255.f, 0.f, 0.f), buf);
                }

                ImGui::End();

            }
        }
    }
}

#else
void DebugService::DrawComponentDebugView()
{
}
#endif
