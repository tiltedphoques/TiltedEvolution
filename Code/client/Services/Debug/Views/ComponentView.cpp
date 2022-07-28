
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

// TODO: ft

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

void DebugService::SetDebugId(const uint32_t aFormId) noexcept
{
    m_formId = aFormId;
}

void DebugService::DrawComponentDebugView()
{
    auto view = m_world.view<FormIdComponent>();

    if (!m_formId)
        return;

    auto entityIt = std::find_if(view.begin(), view.end(), [id = m_formId, view](auto entity) {
        return view.get<FormIdComponent>(entity).Id == id;
    });

    if (entityIt == view.end())
        return;

    auto entity = *entityIt;

    if (auto* pObject = Cast<TESObjectREFR>(TESForm::GetById(m_formId)))
    {
        ImVec2 screenPos{};
        if (DrawInWorldSpace(pObject, screenPos))
        {
            ImGui::SetNextWindowPos(screenPos);
            ImGui::Begin("Component debug");

            if (auto serverIdRes = Utils::GetServerId(entity))
            {
                ImGui::InputScalar("Server ID", ImGuiDataType_U32, &(*serverIdRes), 0, 0, "%" PRIx32,
                                   ImGuiInputTextFlags_CharsHexadecimal);
            }

            if (auto* pComponent = m_world.try_get<LocalComponent>(entity))
            {
                if (ImGui::CollapsingHeader("LocalComponent"))
                {
                    ImGui::InputScalar("Is dead?", ImGuiDataType_U8, &pComponent->IsDead, 0, 0, "%" PRIx8,
                                       ImGuiInputTextFlags_CharsHexadecimal);
                }
            }

            if (auto* pComponent = m_world.try_get<LocalAnimationComponent>(entity))
            {
                if (ImGui::CollapsingHeader("LocalAnimationComponent"))
                {
                    int actions = int(pComponent->Actions.size());
                    ImGui::InputInt("Number of actions", &actions, 0, 0, ImGuiInputTextFlags_ReadOnly);
                }
            }

            if (auto* pComponent = m_world.try_get<RemoteComponent>(entity))
            {
                if (ImGui::CollapsingHeader("RemoteComponent"))
                {
                    ImGui::InputScalar("Cached ref ID", ImGuiDataType_U32, &pComponent->CachedRefId, 0, 0,
                                       "%" PRIx32, ImGuiInputTextFlags_CharsHexadecimal);
                }
            }

            if (auto* pComponent = m_world.try_get<InterpolationComponent>(entity))
            {
                if (ImGui::CollapsingHeader("InterpolationComponent"))
                {
                    ImGui::Text("%f,%f,%f\n", pComponent->Position.x, pComponent->Position.y,
                                pComponent->Position.z);
                }
            }

            if (auto* pComponent = m_world.try_get<FaceGenComponent>(entity))
            {
                if (ImGui::CollapsingHeader("FaceGenComponent"))
                {
                    for (auto x : pComponent->FaceTints.Entries)
                    {
                        ImGui::Text("Alpha %f, Color %u, Type %u\n", x.Alpha, x.Color, x.Type);
                    }
                }
            }

            if (auto* pComponent = m_world.try_get<RemoteAnimationComponent>(entity))
            {
                if (ImGui::CollapsingHeader("RemoteAnimationComponent"))
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

#else
void DebugService::DrawComponentDebugView()
{
}
#endif
