#include <Havok/hkbStateMachine.h>
#include <Structs/AnimationGraphDescriptorManager.h>

#include <Havok/BShkbAnimationGraph.h>
#include <Havok/BShkbHkxDB.h>
#include <Havok/hkbBehaviorGraph.h>

#include <Services/ImguiService.h>
#include <Services/DebugService.h>
#include <Services/TransportService.h>
#include <Services/PapyrusService.h>
#include <Services/QuestService.h>

#include <Events/UpdateEvent.h>
#include <Events/DialogueEvent.h>

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

#include <Messages/RequestRespawn.h>

#include <Interface/UI.h>
#include <Interface/IMenu.h>

#if TP_SKYRIM64
#include <EquipManager.h>
#include <Games/Skyrim/BSGraphics/BSGraphicsRenderer.h>
#include <Games/Skyrim/DefaultObjectManager.h>
#include <Games/Skyrim/Forms/TESAmmo.h>
#include <Games/Skyrim/Misc/InventoryEntry.h>
#include <Games/Skyrim/Misc/MiddleProcess.h>
#include <Games/Skyrim/Interface/UI.h>
#endif

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

void __declspec(noinline) DebugService::PlaceActorInWorld() noexcept
{
    const auto pPlayerBaseForm = static_cast<TESNPC*>(PlayerCharacter::Get()->baseForm);

    auto pActor = Actor::Create(pPlayerBaseForm);

    Inventory inventory = PlayerCharacter::Get()->GetActorInventory();
    pActor->SetActorInventory(inventory);

    m_actors.emplace_back(pActor);
}

DebugService::DebugService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport,
                         ImguiService& aImguiService)
    : m_dispatcher(aDispatcher), m_transport(aTransport), m_world(aWorld)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&DebugService::OnUpdate>(this);
    m_drawImGuiConnection = aImguiService.OnDraw.connect<&DebugService::OnDraw>(this);
    m_actorSpokeConnection = m_dispatcher.sink<DialogueEvent>().connect<&DebugService::OnActorSpokeEvent>(this);
}

void DebugService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    static std::atomic<bool> s_f8Pressed = false;
    static std::atomic<bool> s_f7Pressed = false;
    static std::atomic<bool> s_f6Pressed = false;

    if (GetAsyncKeyState(VK_F6))
    {
        if (!s_f6Pressed)
        {
            s_f6Pressed = true;

            static char s_address[256] = "127.0.0.1:10578";
            if (!m_transport.IsOnline())
                m_transport.Connect(s_address);
            else
                m_transport.Close();
        }
    }
    else
        s_f6Pressed = false;

    if (GetAsyncKeyState(VK_F7))
    {
        if (!s_f7Pressed)
        {
            s_f7Pressed = true;

            static char s_address[256] = "de.playtogether.gg:10100";
            if (!m_transport.IsOnline())
                m_transport.Connect(s_address);
            else
                m_transport.Close();
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
        }
    }
    else
        s_f8Pressed = false;
}

void DebugService::OnActorSpokeEvent(const DialogueEvent& acEvent) noexcept
{
    m_spokenActorId = acEvent.ActorID;
    m_voiceFileName = acEvent.VoiceFile;

    spdlog::debug("Actor spoke, id: {:X}, file: {}", acEvent.ActorID, acEvent.VoiceFile.c_str());
}

uint64_t DebugService::DisplayGraphDescriptorKey(BSAnimationGraphManager* pManager) noexcept
{
    auto hash = pManager->GetDescriptorKey();
    auto pDescriptor = AnimationGraphDescriptorManager::Get().GetDescriptor(hash);

    spdlog::info("Key: {}", hash);
    std::cout << "uint64_t key = " << hash << ";" << std::endl;
    if (!pDescriptor)
        spdlog::error("Descriptor key not found");

    return hash;
}

static bool g_enableAnimWindow{false};
static bool g_enableInventoryWindow{false};
static bool g_enableNetworkWindow{false};
static bool g_enableFormsWindow{false};
static bool g_enablePlayerWindow{false};
static bool g_enableSkillsWindow{false};
static bool g_enablePartyWindow{false};
static bool g_enableActorValuesWindow{false};
static bool g_enableQuestWindow{false};

void DebugService::OnDraw() noexcept
{
    const auto view = m_world.view<FormIdComponent>();
    if (view.empty() || !m_showDebugStuff)
        return;

    DrawEntitiesView();

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
    if (ImGui::BeginMenu("Components"))
    {
        ImGui::MenuItem("Show component list", nullptr, &m_toggleComponentWindow);
        ImGui::MenuItem("Show selected entity in world", nullptr, &m_drawComponentsInWorldSpace);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("UI"))
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

        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Debuggers"))
    {
        ImGui::MenuItem("Network", nullptr, &g_enableNetworkWindow);
        ImGui::MenuItem("Forms", nullptr, &g_enableFormsWindow);
        ImGui::MenuItem("Inventory", nullptr, &g_enableInventoryWindow);
        ImGui::MenuItem("Animations", nullptr, &g_enableAnimWindow);
        ImGui::MenuItem("Player", nullptr, &g_enablePlayerWindow);
        ImGui::MenuItem("Skills", nullptr, &g_enableSkillsWindow);
        ImGui::MenuItem("Party", nullptr, &g_enablePartyWindow);
        ImGui::MenuItem("Quests", nullptr, &g_enableQuestWindow);

        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Misc"))
    {
        if (ImGui::Button("Crash Client"))
        {
            int* m = 0;
            *m = 1338;
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    if (g_enableNetworkWindow)
        DrawNetworkView();
    if (g_enableFormsWindow)
        DrawFormDebugView();
    if (g_enableInventoryWindow)
        DrawContainerDebugView();
    if (g_enableAnimWindow)
        DrawAnimDebugView();
    if (g_enablePlayerWindow)
        DrawPlayerDebugView();
    if (g_enableSkillsWindow)
        DrawSkillView();
    if (g_enablePartyWindow)
        DrawPartyView();
    if (g_enableActorValuesWindow)
        DrawActorValuesView();
    if (g_enableQuestWindow)
        DrawQuestDebugView();

    if (m_toggleComponentWindow)
        DrawComponentDebugView();

    if (m_showBuildTag)
        DrawBuildTag();
}
