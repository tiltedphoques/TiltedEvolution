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
#endif

//#include <Games/Skyrim/>

#include <imgui.h>
#include <inttypes.h>
extern thread_local bool g_overrideFormId;

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

void TestService::RunDiff()
{
    /*
    BSAnimationGraphManager* pManager = nullptr;
    BSAnimationGraphManager* pActorManager = nullptr;

    static Map<uint32_t, uint32_t> s_values;

    if (m_actors.empty())
        return;

    auto pActor = m_actors[0];

    AnimationVariables vars;

    PlayerCharacter::Get()->SaveAnimationVariables(vars);
    pActor->LoadAnimationVariables(vars);

    if (PlayerCharacter::Get()->animationGraphHolder.GetBSAnimationGraph(&pManager) &&
    pActor->animationGraphHolder.GetBSAnimationGraph(&pActorManager))
    {
        if (pManager->animationGraphIndex < pManager->animationGraphs.size)
        {
            const auto pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);
            const auto pActorGraph = pActorManager->animationGraphs.Get(pActorManager->animationGraphIndex);
            if (pGraph && pActorGraph)
            {
                const auto pDb = pGraph->hkxDB;
                const auto pBuckets = pDb->animationVariables.buckets;
                const auto pVariableSet = pGraph->behaviorGraph->animationVariables;
                const auto pActorVariableSet = pActorGraph->behaviorGraph->animationVariables;

                auto pDescriptor =
                    AnimationGraphDescriptorManager::Get().GetDescriptor(pGraph->behaviorGraph->stateMachine->name);

                if (pBuckets && pVariableSet && pActorVariableSet)
                {
                    for (auto i = 0u; i < pVariableSet->size; ++i)
                    {
                        //if (pVariableSet->data[i] != pActorVariableSet->data[i])
                            //spdlog::info("Diff {} expected: {} got: {}", i, pVariableSet->data[i],
    pActorVariableSet->data[i]);

                        auto itor = s_values.find(i);
                        if (itor == std::end(s_values))
                        {
                            s_values[i] = pVariableSet->data[i];

                            if (!pDescriptor->IsSynced(i))
                            {
                                spdlog::info("Variable {} initialized to f: {} i: {}", i,
    *(float*)&pVariableSet->data[i],
                                             *(int32_t*)&pVariableSet->data[i]);
                            }
                        }
                        else if (itor->second != pVariableSet->data[i] && !pDescriptor->IsSynced(i))
                        {
                            spdlog::warn("Variable {} changed to f: {} i: {}", i, *(float*)&pVariableSet->data[i],
                                         *(int32_t*)&pVariableSet->data[i]);

                            s_values[i] = pVariableSet->data[i];
                            //itor->second = pVariableSet->data[i];
                        }
                    }
                }
            }
        }

        pManager->Release();
    }
    */
}

TestService::~TestService() noexcept = default;

void TestService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    static std::atomic<bool> s_f8Pressed = false;
    static std::atomic<bool> s_f7Pressed = false;

    RunDiff();

    if (GetAsyncKeyState(VK_F7))
    {
        if (!s_f7Pressed)
        {
            s_f7Pressed = true;

            static char s_address[256] = "127.0.0.1:10578";
            m_transport.Connect(s_address);
        }
    }

    if (GetAsyncKeyState(VK_F3) & 0x01)
    {
        m_showDebugStuff = !m_showDebugStuff;
    }

    if (GetAsyncKeyState(VK_F8))
    {
        if (!s_f8Pressed)
        {
            s_f8Pressed = true;

            auto* pActor = (Actor*)TESForm::GetById(0xFF000DA5);
            pActor->SetWeaponDrawnEx(true);

            // PlaceActorInWorld();

            /*
            const auto pPlayerBaseForm = static_cast<TESNPC*>(PlayerCharacter::Get()->baseForm);

            //const auto pNpc = TESNPC::Create(data, pPlayerBaseForm->GetChangeFlags());
            auto pActor = Actor::Create(pPlayerBaseForm);
            pActor->SaveInventory(0);

        #if TP_SKYRIM64
            auto& objManager = DefaultObjectManager::Get();
            spdlog::info(objManager.isSomeActionReady);
        #endif

            TP_ASSERT(0, "{}", 5)
            */
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
    ImGui::EndMainMenuBar();

    if (g_EnableAnimWindow)
        DrawAnimDebugView();

    if (m_toggleComponentWindow)
        DrawComponentDebugView();
}
