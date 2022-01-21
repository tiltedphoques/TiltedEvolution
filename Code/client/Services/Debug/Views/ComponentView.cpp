
#include <BSGraphics/BSGraphicsRenderer.h>
#include <Camera/PlayerCamera.h>
#include <Games/Skyrim/Forms/TESForm.h>
#include <Games/Skyrim/NetImmerse/NiCamera.h>
#include <Games/Skyrim/TESObjectREFR.h>
#include <NetImmerse/NiCamera.h>
#include <inttypes.h>

#include <World.h>
#include <services/TestService.h>

#include <imgui.h>

// Note(Force): Here for now, we decide on a proper place for this later.
namespace
{
static float (*guimatrix)[4][4] = nullptr;
static NiRect<float>* guiport = nullptr;

static TiltedPhoques::Initializer s_Init([]() {
    POINTER_FALLOUT4(float[4][4], s_matrix, 0x145A66AA0 - 0x140000000);
    POINTER_FALLOUT4(NiRect<float>, s_port, 0x145A66B30 - 0x140000000);

    POINTER_SKYRIMSE(float[4][4], s_matrix, 0x142FE75F0 - 0x140000000);
    POINTER_SKYRIMSE(NiRect<float>, s_port, 0x142FE8B98 - 0x140000000);

    guimatrix = s_matrix.Get();
    guiport = s_port.Get();
});

bool HUD_WorldPtToScreenPt3(NiPoint3* in, NiPoint3* out)
{
    return NiCamera::WorldPtToScreenPt3(reinterpret_cast<float*>(guimatrix), guiport, in, &out->x, &out->y, &out->z,
                                        1e-5f);
}
} // namespace

static void DrawInWorldSpace(TESObjectREFR* apRefr)
{
    auto calcObjectHeight = [&]() {
        auto boundMax = apRefr->GetBoundMax();
        return boundMax.z - apRefr->GetBoundMin().z;
    };

    // Scale up to the top of the entity.
    auto pos = apRefr->position;
    pos.z -= calcObjectHeight();

    // Note(Force): In truth this is a reference, but to ensure the compiler generates the right code
    // we pass it in as a pointer
    NiPoint3 screenPoint{};
    HUD_WorldPtToScreenPt3(&pos, &screenPoint);

    // Here just for validation that the values are semi correct.
    // RECT rect{};
    // GetWindowRect(GetForegroundWindow(), &rect);

    // Calculate window collision bounds.
    auto* pViewport = BSGraphics::GetMainWindow();
    NiRect<float> bounds = {
        .left = static_cast<float>(pViewport->iWindowX),
        .right = static_cast<float>(pViewport->iWindowX + pViewport->uiWindowWidth),
        .top = static_cast<float>(pViewport->iWindowY),
        .bottom = static_cast<float>(pViewport->iWindowY + pViewport->uiWindowHeight),
    };

    // translate to screen
    ImVec2 screenPos = ImVec2{
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
        ImGui::SetNextWindowPos(screenPos);
        ImGui::Begin("AttachedWindow");
        ImGui::Button("Kill Force67");
        ImGui::End();
    }
}

// TODO(Force): Net Histrogram (waves)
// Engine stuff.
// Fix cursor.

static bool g_DrawComponentsInWorldSpace{false};
static TESForm* g_pSelectedLocalTESForm{nullptr};
static TESForm* g_pSelectedRemoteTESForm{nullptr};

#if 0
void DrawRemoteComponent(const RemoteComponent& acComponent, const InterpolationComponent& acInterpolationc)
{
    ImGui::Begin("Entity");
    ImGui::InputScalar("Server ID", ImGuiDataType_U32, &acComponent.Id, 0, 0, "%" PRIx32,
                       ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputScalar("Cached ref ID", ImGuiDataType_U32, &acComponent.CachedRefId, 0, 0, "%" PRIx32,
                       ImGuiInputTextFlags_CharsHexadecimal);

    auto& interpolationComponent = invisibleView.get<InterpolationComponent>(entity);
    ImGui::InputFloat("Position x", &interpolationComponent.Position.x, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat("Position y", &interpolationComponent.Position.y, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat("Position z", &interpolationComponent.Position.z, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::End();
}
#endif

void TestService::DrawComponentDebugView()
{
    ImGui::MenuItem("Visualize Components", nullptr, &g_DrawComponentsInWorldSpace);

    if (g_DrawComponentsInWorldSpace)
    {
        if (g_pSelectedLocalTESForm)
        {
            #if 0
            auto invisibleView =
                m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent, FormIdComponent>();

            ImGui::Begin("Entity");
            ImGui::InputScalar("Server ID", ImGuiDataType_U32, &acComponent.Id, 0, 0, "%" PRIx32,
                               ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::InputScalar("Cached ref ID", ImGuiDataType_U32, &acComponent.CachedRefId, 0, 0, "%" PRIx32,
                               ImGuiInputTextFlags_CharsHexadecimal);

            auto& interpolationComponent = invisibleView.get<InterpolationComponent>(entity);
            ImGui::InputFloat("Position x", &interpolationComponent.Position.x, 0, 0, "%.3f",
                              ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Position y", &interpolationComponent.Position.y, 0, 0, "%.3f",
                              ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Position z", &interpolationComponent.Position.z, 0, 0, "%.3f",
                              ImGuiInputTextFlags_ReadOnly);
            ImGui::End();
            #endif
        }
    }

    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Invisible"))
        {
            ImGui::BeginChild("Invisible components", ImVec2(0, 100), true);

            static uint32_t s_selectedInvisibleId = 0;
            static uint32_t s_selectedInvisible = 0;

            auto invisibleView =
                m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent, FormIdComponent>();
            Vector<entt::entity> entities(invisibleView.begin(), invisibleView.end());

            int i = 0;
            for (auto entity : entities)
            {
                auto& remoteComponent = invisibleView.get<RemoteComponent>(entity);
                auto& interpolationComponent = invisibleView.get<InterpolationComponent>(entity);

                char buffer[32];
                if (ImGui::Selectable(itoa(remoteComponent.Id, buffer, 16),
                                      s_selectedInvisibleId == remoteComponent.Id))
                    s_selectedInvisibleId = remoteComponent.Id;

                if (s_selectedInvisibleId == remoteComponent.Id)
                    s_selectedInvisible = i;

                ++i;
            }

            ImGui::EndChild();

            if (s_selectedInvisible < entities.size())
            {
                auto entity = entities[s_selectedInvisible];
                //auto& remoteComponent = invisibleView.get<RemoteComponent>(entity);
                auto& formComponent = invisibleView.get<FormIdComponent>(entity);
                if (auto* pEntity = TESForm::GetById(formComponent.Id))
                {
                    g_pSelectedLocalTESForm = pEntity;
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Remote"))
        {
            ImGui::BeginChild("Remote components", ImVec2(0, 100), true);

            static uint32_t s_selectedRemoteId = 0;
            static uint32_t s_selectedRemote = 0;

            auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
            Vector<entt::entity> entities(remoteView.begin(), remoteView.end());

            int i = 0;
            for (auto entity : entities)
            {
                auto& remoteComponent = remoteView.get<RemoteComponent>(entity);

                char buffer[32];
                if (ImGui::Selectable(itoa(remoteComponent.Id, buffer, 16), s_selectedRemoteId == remoteComponent.Id))
                    s_selectedRemoteId = remoteComponent.Id;

                if (s_selectedRemoteId == remoteComponent.Id)
                    s_selectedRemote = i;

                ++i;
            }

            ImGui::EndChild();

            if (s_selectedRemote < entities.size())
            {
                auto entity = entities[s_selectedRemote];

                auto& remoteComponent = remoteView.get<RemoteComponent>(entity);
                ImGui::InputScalar("Server ID", ImGuiDataType_U32, &remoteComponent.Id, 0, 0, "%" PRIx32,
                                   ImGuiInputTextFlags_CharsHexadecimal);
                ImGui::InputScalar("Cached ref ID", ImGuiDataType_U32, &remoteComponent.CachedRefId, 0, 0, "%" PRIx32,
                                   ImGuiInputTextFlags_CharsHexadecimal);

                auto& formComponent = remoteView.get<FormIdComponent>(entity);
                if (auto* pEntity = TESForm::GetById(formComponent.Id))
                {
                    g_pSelectedLocalTESForm = pEntity;
                }
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
