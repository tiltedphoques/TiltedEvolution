#include <TiltedOnlinePCH.h>

#include <Havok/hkbStateMachine.h>
#include <Structs/AnimationGraphDescriptorManager.h>

#include <Havok/BShkbHkxDB.h>
#include <Havok/hkbBehaviorGraph.h>
#include <Havok/BShkbAnimationGraph.h>

#include <Services/TestService.h>
#include <Services/ImguiService.h>
#include <Services/TransportService.h>

#include <Events/UpdateEvent.h>

#include <Games/References.h>

#include <Forms/TESQuest.h>
#include <BSAnimationGraphManager.h>
#include <Forms/TESFaction.h>

#include <Forms/TESNPC.h>

#include <Components.h>
#include <World.h>

#include <Games/TES.h>
#include <Forms/TESWorldSpace.h>
#include <Forms/TESObjectCELL.h>

#include <imgui.h>
#include <inttypes.h>

extern thread_local bool g_overrideFormId;

void __declspec(noinline) TestService::PlaceActorInWorld() noexcept
{
    const auto pPlayerBaseForm = static_cast<TESNPC*>(PlayerCharacter::Get()->baseForm);

    //const auto pNpc = TESNPC::Create(data, pPlayerBaseForm->GetChangeFlags());
    auto pActor = Actor::Create(pPlayerBaseForm);

    pActor->SetInventory(PlayerCharacter::Get()->GetInventory());

    m_actors.emplace_back(pActor);
}

TestService::TestService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport, ImguiService& aImguiService)
    : m_dispatcher(aDispatcher)
    , m_transport(aTransport)
    , m_world(aWorld)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&TestService::OnUpdate>(this);
    m_drawImGuiConnection = aImguiService.OnDraw.connect<&TestService::OnDraw>(this);
}

void TestService::RunDiff()
{
    BSAnimationGraphManager* pManager = nullptr;
    BSAnimationGraphManager* pActorManager = nullptr;

    static Map<uint32_t, uint32_t> s_values;

    if (m_actors.empty())
        return;

    auto pActor = m_actors[0];

    AnimationVariables vars;

    PlayerCharacter::Get()->SaveAnimationVariables(vars);
    pActor->LoadAnimationVariables(vars);

    if (PlayerCharacter::Get()->animationGraphHolder.GetBSAnimationGraph(&pManager) && pActor->animationGraphHolder.GetBSAnimationGraph(&pActorManager))
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
                        if (pVariableSet->data[i] != pActorVariableSet->data[i])
                            spdlog::info("Diff {} expected: {} got: {}", i, pVariableSet->data[i], pActorVariableSet->data[i]);

                        /*auto itor = s_values.find(i);
                        if (itor == std::end(s_values))
                        {
                            s_values[i] = pVariableSet->data[i];

                            if (!pDescriptor->IsSynced(i))
                            {
                                spdlog::info("Variable {} initialized to f: {} i: {}", i, *(float*)&pVariableSet->data[i],
                                             *(int32_t*)&pVariableSet->data[i]);
                            }
                        }
                        else if (itor->second != pVariableSet->data[i] && !pDescriptor->IsSynced(i))
                        {
                            spdlog::info("Variable {} changed to f: {} i: {}", i, *(float*)&pVariableSet->data[i],
                                         *(int32_t*)&pVariableSet->data[i]);

                            itor->second = pVariableSet->data[i];
                        }*/


                    }
                }
            }
        }

        pManager->Release();
    }
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

void TestService::OnDraw() noexcept
{
    static uint32_t fetchFormId;
    static Actor* pFetchActor = 0;

    const auto view = m_world.view<FormIdComponent>();
    if (view.empty())
        return;

    ImGui::Begin("Server");

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
        {
            m_transport.Connect(s_address);
        }
    }

    ImGui::End();

    ImGui::Begin("Player");

    auto pPlayer = PlayerCharacter::Get();
    if (pPlayer)
    {
        auto pLeftWeapon = pPlayer->GetEquippedWeapon(0);
        auto pRightWeapon = pPlayer->GetEquippedWeapon(1);

        uint32_t leftId = pLeftWeapon ? pLeftWeapon->formID : 0;
        uint32_t rightId = pRightWeapon ? pRightWeapon->formID : 0;

        ImGui::InputScalar("Left Item", ImGuiDataType_U32, (void*)&leftId, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("Right Item", ImGuiDataType_U32, (void*)&rightId, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);

        leftId = pPlayer->magicItems[0] ? pPlayer->magicItems[0]->formID : 0;
        rightId = pPlayer->magicItems[1] ? pPlayer->magicItems[1]->formID : 0;

        ImGui::InputScalar("Right Magic", ImGuiDataType_U32, (void*)&rightId, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("Left Magic", ImGuiDataType_U32, (void*)&leftId, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);

#if TP_SKYRIM
        uint32_t shoutId = pPlayer->equippedShout ? pPlayer->equippedShout->formID : 0;

        ImGui::InputScalar("Shout", ImGuiDataType_U32, (void*)&shoutId, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
#endif  

        auto pWorldSpace = pPlayer->GetWorldSpace();
        if (pWorldSpace)
        {
            auto worldFormId = pWorldSpace->formID;
            ImGui::InputScalar("Worldspace", ImGuiDataType_U32, (void*)&worldFormId, nullptr, nullptr, "%" PRIx32, 
                               ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        }

        auto pCell = pPlayer->GetParentCell();
        if (pCell)
        {
            auto cellFormId = pCell->formID;
            ImGui::InputScalar("Cell Id", ImGuiDataType_U32, (void*)&cellFormId, nullptr, nullptr, "%" PRIx32,
                               ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        }

        const auto playerParentCell = pPlayer->parentCell;
        if (playerParentCell)
        {
            ImGui::InputScalar("Player parent cell", ImGuiDataType_U32, (void*)&playerParentCell->formID, nullptr, nullptr, "%" PRIx32,
                               ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        }

        auto* pTES = TES::Get();
        if (pTES)
        {
            int32_t playerGrid[2] = {pTES->currentGridX, pTES->currentGridY};
            int32_t centerGrid[2] = {pTES->centerGridX, pTES->centerGridY};

            ImGui::InputInt2("Player grid", playerGrid, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputInt2("Center grid", centerGrid, ImGuiInputTextFlags_ReadOnly);
        }
    }

    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Actor lookup");

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, &fetchFormId, 0, 0, "%" PRIx32, ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Look up"))
    {
        if (fetchFormId)
        {
            auto* pFetchForm = TESForm::GetById(fetchFormId);
            if (pFetchForm)
                pFetchActor = RTTI_CAST(pFetchForm, TESForm, Actor);
        }
    }

    if (pFetchActor)
    {
#if TP_SKYRIM64
        const auto* pNpc = RTTI_CAST(pFetchActor->baseForm, TESForm, TESNPC);
        if (pNpc && pNpc->fullName.value.data)
        {
            char name[256];
            sprintf_s(name, std::size(name), "%s", pNpc->fullName.value.data);
            ImGui::InputText("Name", name, std::size(name), ImGuiInputTextFlags_ReadOnly);
        }
#endif

        ImGui::InputScalar("Memory address", ImGuiDataType_U64, (void*)&pFetchActor, 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);

        ImGui::InputInt("Game Id", (int*)&pFetchActor->formID, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputFloat3("Position", pFetchActor->position.AsArray(), "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat3("Rotation", pFetchActor->rotation.AsArray(), "%.3f", ImGuiInputTextFlags_ReadOnly);
        int isDead = int(pFetchActor->IsDead());
        ImGui::InputInt("Is dead?", &isDead, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

        auto* pWorldSpace = pFetchActor->GetWorldSpace();
        if (pWorldSpace)
        {
            auto worldFormId = pWorldSpace->formID;
            ImGui::InputScalar("Actor Worldspace", ImGuiDataType_U32, (void*)&worldFormId, nullptr, nullptr, "%" PRIx32,
                               ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        }

        auto* pCell = pFetchActor->GetParentCell();
        if (pCell)
        {
            auto cellFormId = pCell->formID;
            ImGui::InputScalar("Actor Cell Id", ImGuiDataType_U32, (void*)&cellFormId, nullptr, nullptr, "%" PRIx32,
                               ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        }
    }

    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Component view");

    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Invisible"))
        {
            ImGui::BeginChild("Invisible components", ImVec2(0, 100), true);

            static uint32_t s_selectedInvisibleId = 0;
            static uint32_t s_selectedInvisible = 0;

            auto invisibleView = m_world.view<RemoteComponent, InterpolationComponent, RemoteAnimationComponent>(
                entt::exclude<FormIdComponent>);
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

                auto& remoteComponent = invisibleView.get<RemoteComponent>(entity);
                ImGui::InputScalar("Server ID", ImGuiDataType_U32, &remoteComponent.Id, 0, 0, "%" PRIx32,
                                   ImGuiInputTextFlags_CharsHexadecimal);
                ImGui::InputScalar("Cached ref ID", ImGuiDataType_U32, &remoteComponent.CachedRefId, 0, 0, "%" PRIx32,
                                   ImGuiInputTextFlags_CharsHexadecimal);

                auto& interpolationComponent = invisibleView.get<InterpolationComponent>(entity);
                ImGui::InputFloat("Position x", &interpolationComponent.Position.x, 0, 0, "%.3f",
                                  ImGuiInputTextFlags_ReadOnly);
                ImGui::InputFloat("Position y", &interpolationComponent.Position.y, 0, 0, "%.3f",
                                  ImGuiInputTextFlags_ReadOnly);
                ImGui::InputFloat("Position z", &interpolationComponent.Position.z, 0, 0, "%.3f",
                                  ImGuiInputTextFlags_ReadOnly);
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Remote"))
        {
            ImGui::BeginChild("Remote components", ImVec2(0, 100), true);

            static uint32_t s_selectedRemoteId = 0;
            static uint32_t s_selectedRemote = 0;

            auto remoteView = m_world.view<RemoteComponent>();
            Vector<entt::entity> entities(remoteView.begin(), remoteView.end());

            int i = 0;
            for (auto entity : entities)
            {
                auto& remoteComponent = remoteView.get<RemoteComponent>(entity);

                char buffer[32];
                if (ImGui::Selectable(itoa(remoteComponent.Id, buffer, 16),
                                      s_selectedRemoteId == remoteComponent.Id))
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
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}


