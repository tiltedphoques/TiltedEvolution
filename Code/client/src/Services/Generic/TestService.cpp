#include "Services/PapyrusService.h"

#include <Havok/hkbStateMachine.h>
#include <Structs/AnimationGraphDescriptorManager.h>

#include <Havok/BShkbAnimationGraph.h>
#include <Havok/BShkbHkxDB.h>
#include <Havok/hkbBehaviorGraph.h>

#include <Services/ImguiService.h>
#include <Services/TestService.h>
#include <Services/TransportService.h>

#include <Events/UpdateEvent.h>

#include <Games/References.h>

#include <BSAnimationGraphManager.h>
#include <Forms/TESFaction.h>
#include <Forms/TESQuest.h>

#include <Forms/TESNPC.h>

#include <Components.h>
#include <World.h>

#include <imgui.h>

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
                        if (pVariableSet->data[i] != pActorVariableSet->data[i])
                            spdlog::info("Diff {} expected: {} got: {}", i, pVariableSet->data[i],
                                         pActorVariableSet->data[i]);

                        /*auto itor = s_values.find(i);
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
    static std::atomic<bool> s_gPressed = false;

    RunDiff();

    if (GetAsyncKeyState(0x47)) // G Key
    {
        if (!s_gPressed)
        {
            using StringUtil = GameVM;
            using UI = GameVM;
            s_gPressed = true;
            // spdlog::log(spdlog::level::info, "trying to save via papyrus");
            for (const auto& functionName : World::Get().ctx<PapyrusService>().RegisteredFunctionNames())
            {
                spdlog::log(spdlog::level::info, functionName);
            }

            PAPYRUS_FUNCTION(bool, UI, IsTextInputEnabled);
            bool textInputEnabled = s_pIsTextInputEnabled(SkyrimVM::Get());
            std::cout << "textInputEnabled: " << textInputEnabled << std::endl;

            GLOBAL_PAPYRUS_FUNCTION(bool, UI, IsMenuOpen, BSFixedString)
            char* rawString = new char[18];
            strncpy(rawString, "InventoryMenu", 18);
            std::cout << "strlen: " << strlen(rawString) << std::endl;
            BSFixedString selfMadeString(rawString);
            bool is_open = s_pIsMenuOpen("InventoryMenu");
            std::cout << "is_open: " << is_open << std::endl;

            // PAPYRUS_FUNCTION(bool, StringUtil, GetLength, BSFixedString);
            /*using GlobalTFunction = bool(__fastcall * )(BSScript::IVirtualMachine*,  BSFixedString);



            GlobalTFunction func(
                reinterpret_cast<GlobalTFunction>(World::Get().ctx<PapyrusService>().Get("StringUtil", "GetLength")));*/

            // auto length = func(GameVM::Get()->virtualMachine, selfMadeString);
            // bool length = s_pGetLength(SkyrimVM::Get(), selfMadeString);

            GLOBAL_PAPYRUS_FUNCTION(uint64_t, StringUtil, GetLength, BSFixedString);

            std::cout << "length: " << s_pGetLength(BSFixedString("InventoryMenu")) << std::endl;

            using Game = GameVM;
            PAPYRUS_FUNCTION(void, Game, ForceThirdPerson);
            s_pForceThirdPerson(SkyrimVM::Get());

            /* PAPYRUS_FUNCTION(Actor*, Game, GetPlayer);
             Actor* player = s_pGetPlayer(SkyrimVM::Get());
             const char* name = player->baseForm->GetName();*/

            // std::cout << "name: " << name << std::endl;

            /* PAPYRUS_FUNCTION(void, Game, SaveGame, BSFixedString*);
             s_pSaveGame(SkyrimVM::Get(), &selfMadeString);*/

            PAPYRUS_FUNCTION(void, Game, TriggerScreenBlood, uint32_t);
            s_pTriggerScreenBlood(SkyrimVM::Get(), 5);

            PAPYRUS_FUNCTION(void, Game, AddPerkPoints, uint32_t);
            s_pAddPerkPoints(SkyrimVM::Get(), 3);

            struct returnT
            {
                // wchar_t* ptr;
                uint8_t data[8];
                uint8_t* ptr;
            };

            std::cout << "sizeof returnT :" << sizeof(returnT) << std::endl;
            PAPYRUS_FUNCTION(returnT, StringUtil, AsChar, uint64_t);
            returnT asChar5 = s_pAsChar(SkyrimVM::Get(), 5);
            returnT asChar5b = s_pAsChar(SkyrimVM::Get(), 5);
            returnT asChar65 = s_pAsChar(SkyrimVM::Get(), 65);
            returnT asChar133713371337 = s_pAsChar(SkyrimVM::Get(), 13371337);
            std::cout << asChar5.data[0] << std::endl;
            std::cout << asChar65.data[0] << std::endl;

            std::cout << asChar5b.data[0] << std::endl;

            std::cout << asChar133713371337.data[0] << std::endl;

            // spdlog::info("aschar result: {}", );

            /*        PAPYRUS_FUNCTION(bool, Game, IsMenuOpen, const char*);
                    bool inventoryIsOpen = s_pIsMenuOpen(SkyrimVM::Get(), rawString);
                    spdlog::info("inventory is open: {:X}", inventoryIsOpen);*/

            PAPYRUS_FUNCTION(uint64_t, Game, GetPerkPoints);
            uint64_t perkPoints = s_pGetPerkPoints(SkyrimVM::Get());
            spdlog::info("perk points: {:X}", perkPoints);
        }
    }
    else
        s_gPressed = false;

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

        ImGui::InputScalar("Left Item", ImGuiDataType_U32, (void*)&leftId, nullptr, nullptr, nullptr,
                           ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("Right Item", ImGuiDataType_U32, (void*)&rightId, nullptr, nullptr, nullptr,
                           ImGuiInputTextFlags_ReadOnly);

        leftId = pPlayer->magicItems[0] ? pPlayer->magicItems[0]->formID : 0;
        rightId = pPlayer->magicItems[1] ? pPlayer->magicItems[1]->formID : 0;

        ImGui::InputScalar("Right Magic", ImGuiDataType_U32, (void*)&rightId, nullptr, nullptr, nullptr,
                           ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("Left Magic", ImGuiDataType_U32, (void*)&leftId, nullptr, nullptr, nullptr,
                           ImGuiInputTextFlags_ReadOnly);

#if TP_SKYRIM
        uint32_t shoutId = pPlayer->equippedShout ? pPlayer->equippedShout->formID : 0;

        ImGui::InputScalar("Shout", ImGuiDataType_U32, (void*)&shoutId, nullptr, nullptr, nullptr,
                           ImGuiInputTextFlags_ReadOnly);
#endif
    }

    ImGui::End();
}
