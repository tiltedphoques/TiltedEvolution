
#include <Components.h>
#include <World.h>
#include <imgui.h>
#include <inttypes.h>
#include <services/DebugService.h>

#include <BSGraphics/BSGraphicsRenderer.h>
#include <Camera/PlayerCamera.h>
#include <Games/Skyrim/Forms/TESForm.h>
#include <Games/Skyrim/Interface/Menus/HUDMenuUtils.h>
#include <Games/Skyrim/NetImmerse/NiCamera.h>
#include <Games/Skyrim/TESObjectREFR.h>
#include <NetImmerse/NiCamera.h>

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
    pos.z += apRefr->GetHeight();

    NiPoint3 screenPoint{};
    HUDMenuUtils::WorldPtToScreenPt3(pos, screenPoint);
    // Calculate window collision bounds.
    auto* pViewport = BSGraphics::GetMainWindow();

    // Translate to screen
    const ImVec2 screenPos = ImVec2{
        (pViewport->uiWindowWidth * screenPoint.x),
        (pViewport->uiWindowHeight * (1.0f - screenPoint.y)),
    };

    const ImVec2 windowSize = {
        static_cast<float>(pViewport->uiWindowWidth),
        static_cast<float>(pViewport->uiWindowHeight),
    };

    auto IsVisible = [](const ImVec2& acVec2, const ImVec2& acScreenSize, float z) {
        return (acVec2.x > 0 && acVec2.x <= acScreenSize.x) &&
               (acVec2.y > 0 && acVec2.y <= acScreenSize.y) && z >= 0;
    };

    if (IsVisible(screenPos, windowSize, screenPoint.z))
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

    auto entityIt = std::find_if(view.begin(), view.end(), [id = m_formId, view](auto entity) { return view.get<FormIdComponent>(entity).Id == id; });

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

            if (auto* pComponent = m_world.try_get<ReplayedActionsDebugComponent>(entity))
            {
                const size_t total = pComponent->ActionsReceivedForReplay.size();
                const auto header = fmt::format("List of Replayed Actions ({})", total);
                if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Actions that were run (replayed) after this remote Actor received spawn data from the server");
                    }
                    String replayedActionsText;
                    for (size_t i = 0; i < pComponent->ActionsReceivedForReplay.size(); ++i)
                    {
                        if (i > 0)
                            replayedActionsText += ", ";
                        replayedActionsText += pComponent->ActionsReceivedForReplay[i].EventName;
                    }
                    ImGui::TextWrapped((replayedActionsText + '.').c_str());
                }
            }

            if (auto serverIdRes = Utils::GetServerId(entity))
            {
                ImGui::InputScalar("Server ID", ImGuiDataType_U32, &(*serverIdRes), 0, 0, "%" PRIx32, ImGuiInputTextFlags_CharsHexadecimal);
            }

            if (auto* pComponent = m_world.try_get<LocalComponent>(entity))
            {
                if (ImGui::CollapsingHeader("LocalComponent"))
                {
                    ImGui::InputScalar("Is dead?", ImGuiDataType_U8, &pComponent->IsDead, 0, 0, "%" PRIx8, ImGuiInputTextFlags_CharsHexadecimal);
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
                    ImGui::InputScalar("Cached ref ID", ImGuiDataType_U32, &pComponent->CachedRefId, 0, 0, "%" PRIx32, ImGuiInputTextFlags_CharsHexadecimal);
                }
            }

            if (auto* pComponent = m_world.try_get<InterpolationComponent>(entity))
            {
                if (ImGui::CollapsingHeader("InterpolationComponent"))
                {
                    ImGui::Text("%f,%f,%f\n", pComponent->Position.x, pComponent->Position.y, pComponent->Position.z);
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
                    ImGui::Text("EventName: %s\nTargetEventName: %s\nState1: %u\nState2: %u", pComponent->LastRanAction.EventName.c_str(), pComponent->LastRanAction.TargetEventName.c_str(), pComponent->LastRanAction.State1, pComponent->LastRanAction.State2);
                }
                // ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 30.f, screenPos,
                //                                         ImColor::ImColor(255.f, 0.f, 0.f), buf);
            }

            ImGui::End();
        }
    }
}
