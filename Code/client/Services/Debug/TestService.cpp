#include <TiltedOnlinePCH.h>

#include <Havok/hkbStateMachine.h>
#include <Structs/AnimationGraphDescriptorManager.h>

#include <Havok/BShkbAnimationGraph.h>
#include <Havok/BShkbHkxDB.h>
#include <Havok/hkbBehaviorGraph.h>

#include <Services/ImguiService.h>
#include <Services/TestService.h>
#include <Services/TransportService.h>

#include <Events/MagicSyncEvent.h>
#include <Events/UpdateEvent.h>

#include <Games/References.h>

#include <BSAnimationGraphManager.h>
#include <Forms/TESFaction.h>
#include <Forms/TESQuest.h>

#include <Forms/BGSAction.h>
#include <Forms/TESIdleForm.h>
#include <Forms/TESNPC.h>
#include <Games/Animation/ActorMediator.h>
#include <Games/Animation/TESActionData.h>
#include <Magic/ActorMagicCaster.h>
#include <Misc/BSFixedString.h>
#include <Structs/ActionEvent.h>

#include <Components.h>
#include <World.h>

#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>
#include <Games/TES.h>

#include <AI/AIProcess.h>

#if TP_SKYRIM64
#include <EquipManager.h>
#include <Games/Skyrim/BSGraphics/BSGraphicsRenderer.h>
#include <Games/Skyrim/DefaultObjectManager.h>
#include <Games/Skyrim/Forms/TESAmmo.h>
#include <Games/Skyrim/Misc/InventoryEntry.h>
#include <Games/Skyrim/Misc/MiddleProcess.h>
#include <Games/Skyrim/Interface/UI.h>
#endif

//#include <Games/Skyrim/>

#include <imgui.h>
#include <inttypes.h>
extern thread_local bool g_overrideFormId;

constexpr char kBuildTag[] = "Build: " BUILD_COMMIT " " BUILD_BRANCH " EVO\nBuilt: " __TIMESTAMP__;
static void DrawBuildTag()
{
#ifndef TP_FALLOUT
    auto* pWindow = BSGraphics::GetMainWindow();
    const ImVec2 coord{50.f, static_cast<float>((pWindow->uiWindowHeight + 25) - 100)};
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), coord,
                                            ImColor::ImColor(255.f, 0.f, 0.f),
                                            kBuildTag);
#endif
}

void __declspec(noinline) TestService::PlaceActorInWorld() noexcept
{
    const auto pPlayerBaseForm = static_cast<TESNPC*>(PlayerCharacter::Get()->baseForm);

    // const auto pNpc = TESNPC::Create(data, pPlayerBaseForm->GetChangeFlags());
    auto pActor = Actor::Create(pPlayerBaseForm);

    pActor->SetInventory(PlayerCharacter::Get()->GetInventory());

    m_actors.emplace_back(pActor);
}

TestService::TestService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport,
                         ImguiService& aImguiService)
    : m_dispatcher(aDispatcher), m_transport(aTransport), m_world(aWorld)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&TestService::OnUpdate>(this);
    m_drawImGuiConnection = aImguiService.OnDraw.connect<&TestService::OnDraw>(this);
}

TestService::~TestService() noexcept = default;

void TestService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    static std::atomic<bool> s_f8Pressed = false;
    static std::atomic<bool> s_f7Pressed = false;

    if (GetAsyncKeyState(VK_F7))
    {
        if (!s_f7Pressed)
        {
            s_f7Pressed = true;

            static char s_address[256] = "127.0.0.1:10578";
            if (!m_transport.IsOnline())
                m_transport.Connect(s_address);
        }
    }
    else
        s_f7Pressed = false;

    if (GetAsyncKeyState(VK_F3) & 0x01)
    {
        m_showDebugStuff = !m_showDebugStuff;
    }

    if (GetAsyncKeyState(VK_F8))
    {
        if (!s_f8Pressed)
        {
            s_f8Pressed = true;

            PlaceActorInWorld();
        }
    }
    else
        s_f8Pressed = false;
}

uint64_t TestService::DisplayGraphDescriptorKey(BSAnimationGraphManager* pManager) noexcept
{
    auto hash = pManager->GetDescriptorKey();
    auto pDescriptor = AnimationGraphDescriptorManager::Get().GetDescriptor(hash);

    spdlog::info("Key: {}", hash);
    std::cout << "uint64_t key = " << hash << ";" << std::endl;
    if (!pDescriptor)
        spdlog::error("Descriptor key not found");

    return hash;
}

static bool g_EnableAnimWindow{false};

void TestService::OnDraw() noexcept
{
    const auto view = m_world.view<FormIdComponent>();
    if (view.empty() || !m_showDebugStuff)
        return;

    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Server"))
    {
        static char s_address[256] = "127.0.0.1:10578";
        ImGui::InputText("Address", s_address, std::size(s_address));

        if (m_transport.IsOnline())
        {
            if (ImGui::Button("Disconnect"))
                m_transport.Close();
        }
        else
        {
            if (ImGui::Button("Connect"))
                m_transport.Connect(s_address);
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Player"))
    {
        DrawPlayerDebugView();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Components"))
    {
        ImGui::MenuItem("Show component list", nullptr, &m_toggleComponentWindow);
        ImGui::MenuItem("Show selected entity in world", nullptr, &m_drawComponentsInWorldSpace);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Containers"))
    {
        DrawContainerDebugView();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Animation"))
    {
        ImGui::MenuItem("Toggle anim window", nullptr, &g_EnableAnimWindow);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Form"))
    {
        DrawFormDebugView();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("UI"))
    {
        ImGui::MenuItem("Show build tag", nullptr, &m_showBuildTag);
        if (ImGui::Button("Create All"))
        {
            //UI::Get()->InstanciateAllMenus();
        }
        if (ImGui::Button("Close All"))
        {
            UI::Get()->CloseAllMenus();
        }
        if (ImGui::Button("Unpause all"))
        {
            UI::Get()->UnPauseAll();
            UI::Get()->UnPauseAll2();
        }

        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    if (g_EnableAnimWindow)
        DrawAnimDebugView();

    if (m_toggleComponentWindow)
        DrawComponentDebugView();

    if (m_showBuildTag)
        DrawBuildTag();
}
