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
#include <Events/MagicSyncEvent.h>

#include <Games/References.h>

#include <Forms/TESQuest.h>
#include <BSAnimationGraphManager.h>
#include <Forms/TESFaction.h>

#include <Forms/TESNPC.h>
#include <Forms/BGSAction.h>
#include <Forms/TESIdleForm.h>
#include <Structs/ActionEvent.h>
#include <Games/Animation/ActorMediator.h>
#include <Games/Animation/TESActionData.h>
#include <Misc/BSFixedString.h>
#include <Magic/ActorMagicCaster.h>

#include <Components.h>
#include <World.h>

#include <Games/TES.h>
#include <Forms/TESWorldSpace.h>
#include <Forms/TESObjectCELL.h>

#include <AI/AIProcess.h>

#if TP_SKYRIM64
#include <Games/Skyrim/Misc/MiddleProcess.h>
#include <Games/Skyrim/Forms/TESAmmo.h>
#include <Games/Skyrim/DefaultObjectManager.h>
#include <Games/Skyrim/Misc/InventoryEntry.h>
#include <EquipManager.h>

#include <ExtraData/ExtraCharge.h>
#include <ExtraData/ExtraCount.h>
#include <ExtraData/ExtraEnchantment.h>
#include <ExtraData/ExtraHealth.h>
#include <ExtraData/ExtraPoison.h>
#include <ExtraData/ExtraPoison.h>
#include <ExtraData/ExtraSoul.h>
#include <ExtraData/ExtraTextDisplayData.h>
#include <ExtraData/ExtraWorn.h>
#include <ExtraData/ExtraWornLeft.h>
#include <Forms/EnchantmentItem.h>
#include <Forms/AlchemyItem.h>
#endif

#include <imgui.h>
#include <inttypes.h>

extern thread_local bool g_overrideFormId;

void __declspec(noinline) TestService::PlaceActorInWorld() noexcept
{
    const auto pPlayerBaseForm = static_cast<TESNPC*>(PlayerCharacter::Get()->baseForm);

    //const auto pNpc = TESNPC::Create(data, pPlayerBaseForm->GetChangeFlags());
    auto pActor = Actor::Create(pPlayerBaseForm);

    //pActor->SetInventory(PlayerCharacter::Get()->GetInventory());

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
                        //if (pVariableSet->data[i] != pActorVariableSet->data[i])
                            //spdlog::info("Diff {} expected: {} got: {}", i, pVariableSet->data[i], pActorVariableSet->data[i]);

                        auto itor = s_values.find(i);
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
    static std::atomic<bool> s_f6Pressed = false;

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

            auto fullContainer = PlayerCharacter::Get()->GetFullContainer();
            spdlog::info("Full container entries: {}", fullContainer.Entries.size());

            auto pActor = m_actors[0];
            auto* pObj = RTTI_CAST(TESForm::GetById(0x139B9), TESForm, TESBoundObject);
            pActor->AddObjectToContainer(pObj, nullptr, 1, nullptr);

            /*
            auto* pActor = (Actor*)TESForm::GetById(0xFF000DA5);
            pActor->SetWeaponDrawnEx(true);

            //PlaceActorInWorld();

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

    if (GetAsyncKeyState(VK_F6))
    {
        if (!s_f6Pressed)
        {
            s_f6Pressed = true;

            PlaceActorInWorld();
        }
    }
    else
        s_f6Pressed = false;
}

uint64_t TestService::DisplayGraphDescriptorKey(BSAnimationGraphManager* pManager) noexcept
{
    auto hash = pManager->GetDescriptorKey();
    auto pDescriptor =
        AnimationGraphDescriptorManager::Get().GetDescriptor(hash);

    spdlog::info("Key: {}", hash);
    std::cout << "uint64_t key = " << hash << ";" << std::endl;
    if (!pDescriptor)
        spdlog::error("Descriptor key not found");

    return hash;
}

void TestService::AnimationDebugging() noexcept
{
    static uint32_t fetchFormId = 0;
    static Actor* pActor = nullptr;
    static Map<uint32_t, uint32_t> s_values;
    static Map<uint32_t, uint32_t> s_reusedValues;
    static Map<uint32_t, short> s_valueTypes; //0 for bool, 1 for float, 2 for int
    static Vector<uint32_t> s_blacklist{};
    static SortedMap<uint32_t, String> s_varMap{};
    static Map<uint64_t, uint32_t> s_cachedKeys{};

    ImGui::Begin("Animation debugging");

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, &fetchFormId, 0, 0, "%" PRIx32, ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Look up"))
    {
        if (fetchFormId)
        {
            auto* pFetchForm = TESForm::GetById(fetchFormId);
            if (pFetchForm)
                pActor = RTTI_CAST(pFetchForm, TESForm, Actor);
        }

        s_values.clear();
        s_reusedValues.clear();
        s_valueTypes.clear();
        s_blacklist.clear();
        s_varMap.clear();
    }

    if (!pActor)
    {
        ImGui::End();
        s_varMap.clear();
        s_values.clear();
        s_reusedValues.clear();
        s_valueTypes.clear();
        s_blacklist.clear();
        return;
    }

    if (ImGui::Button("Clear all"))
    {
        s_varMap.clear();
        s_values.clear();
        s_reusedValues.clear();
        s_valueTypes.clear();
        s_blacklist.clear();
    }

    BSAnimationGraphManager* pManager = nullptr;
    pActor->animationGraphHolder.GetBSAnimationGraph(&pManager);

    if (!pManager)
    {
        ImGui::End();
        s_varMap.clear();
        s_values.clear();
        s_reusedValues.clear();
        s_valueTypes.clear();
        s_blacklist.clear();
        return;
    }

    const auto pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);

    if (!pGraph)
    {
        ImGui::End();
        s_varMap.clear();
        s_values.clear();
        s_reusedValues.clear();
        s_valueTypes.clear();
        s_blacklist.clear();
        return;
    }

    if (s_varMap.empty())
    {
        s_varMap = pManager->DumpAnimationVariables(true);

        auto hash = DisplayGraphDescriptorKey(pManager);

        if (s_cachedKeys.find(hash) != std::end(s_cachedKeys))
            spdlog::warn("Key was detected before! Form ID of last detected actor: {:X}", s_cachedKeys[hash]);
        s_cachedKeys[hash] = pActor->formID;
    }

    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Variables"))
        {
            auto hash = pManager->GetDescriptorKey();
            auto pDescriptor = AnimationGraphDescriptorManager::Get().GetDescriptor(hash);
            if (pDescriptor)
            {
                const auto* pVariableSet = pGraph->behaviorGraph->animationVariables;

                Map<String, bool> bools{};
                Map<String, float> floats{};
                Map<String, int> ints{};

                for (size_t i = 0; i < pDescriptor->BooleanLookUpTable.size(); ++i)
                {
                    const auto idx = pDescriptor->BooleanLookUpTable[i];
                    bools[s_varMap[idx]] = *reinterpret_cast<bool*>(&pVariableSet->data[idx]);
                }

                for (size_t i = 0; i < pDescriptor->FloatLookupTable.size(); ++i)
                {
                    const auto idx = pDescriptor->FloatLookupTable[i];
                    floats[s_varMap[idx]] = *reinterpret_cast<float*>(&pVariableSet->data[idx]);
                }

                for (size_t i = 0; i < pDescriptor->IntegerLookupTable.size(); ++i)
                {
                    const auto idx = pDescriptor->IntegerLookupTable[i];
                    ints[s_varMap[idx]] = *reinterpret_cast<int*>(&pVariableSet->data[idx]);
                }

                if (ImGui::Button("Dump variable values"))
                {
                    std::cout << "Bools: " << std::endl;
                    for (auto& [name, value] : bools)
                    {
                        std::cout << "\t" << name << ": " << value << std::endl;
                    }

                    std::cout << "Floats: " << std::endl;
                    for (auto& [name, value] : floats)
                    {
                        std::cout << "\t" << name << ": " << value << std::endl;
                    }

                    std::cout << "Integers: " << std::endl;
                    for (auto& [name, value] : ints)
                    {
                        std::cout << "\t" << name << ": " << value << std::endl;
                    }
                }

                if (ImGui::CollapsingHeader("Bools", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (auto pair : bools)
                    {
                        auto name = pair.first;
                        auto value = pair.second;
                        int iValue = int(value);
                        ImGui::InputInt(name.c_str(), &iValue, 0, 0, ImGuiInputTextFlags_ReadOnly);
                    }
                }

                if (ImGui::CollapsingHeader("Floats", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (auto pair : floats)
                    {
                        auto name = pair.first;
                        auto value = pair.second;
                        ImGui::InputFloat(name.c_str(), &value, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    }
                }

                if (ImGui::CollapsingHeader("Integers", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (auto pair : ints)
                    {
                        auto name = pair.first;
                        auto value = pair.second;
                        ImGui::InputInt(name.c_str(), &value, 0, 0, ImGuiInputTextFlags_ReadOnly);
                    }
                }
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Blacklist"))
        {
            static uint32_t s_selectedBlacklistVar = 0;
            static uint32_t s_selected = 0;

            ImGui::BeginChild("Blacklisted variables");

            int i = 0;
            for (auto blacklistedVar : s_blacklist)
            {
                if (blacklistedVar >= pGraph->behaviorGraph->animationVariables->size)
                {
                    ++i;
                    continue;
                }

                const auto varName = s_varMap[blacklistedVar];

                char name[256];
                sprintf_s(name, std::size(name), "k%s (%d)", varName.c_str(), blacklistedVar);

                if (ImGui::Selectable(name, s_selectedBlacklistVar == blacklistedVar))
                {
                    s_selectedBlacklistVar = blacklistedVar;
                }

                if (s_selectedBlacklistVar == blacklistedVar)
                {
                    s_selected = i;
                }

                ++i;
            }

            ImGui::EndChild();

            if (s_selected < s_blacklist.size())
            {
                if (ImGui::Button("Delete"))
                {
                    s_blacklist.erase(s_blacklist.begin() + s_selected);
                }
            }

            static uint32_t s_blacklistVar = 0;
            ImGui::InputInt("New blacklist var:", (int*)&s_blacklistVar, 0, 0);

            if (ImGui::Button("Add"))
            {
                s_blacklist.push_back(s_blacklistVar);
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Control"))
        {
            ImGui::InputInt("Animation graph count", (int*)&pManager->animationGraphs.size, 0, 0, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputInt("Animation graph index", (int*)&pManager->animationGraphIndex, 0, 0, ImGuiInputTextFlags_ReadOnly);

            char name[256];
            sprintf_s(name, std::size(name), "%s", pGraph->behaviorGraph->stateMachine->name);
            ImGui::InputText("Graph state machine name", name, std::size(name), ImGuiInputTextFlags_ReadOnly);

            const auto pVariableSet = pGraph->behaviorGraph->animationVariables;

            auto pDescriptor =
                AnimationGraphDescriptorManager::Get().GetDescriptor(pManager->GetDescriptorKey());

            static bool toggleVariableRecord = false;
            if (ImGui::Button("Toggle variable recording"))
            {
                toggleVariableRecord = !toggleVariableRecord;
                spdlog::info("Toggle variable recording: {}", toggleVariableRecord);
            }

            if (pVariableSet && toggleVariableRecord)
            {
                for (auto i = 0u; i < pVariableSet->size; ++i)
                {
                    if (std::find(s_blacklist.begin(), s_blacklist.end(), i) != s_blacklist.end())
                        continue;

                    if (pDescriptor && pDescriptor->IsSynced(i))
                        continue;

                    auto iter = s_values.find(i);
                    if (iter == std::end(s_values))
                    {
                        s_values[i] = pVariableSet->data[i];

                        const auto varName = s_varMap[i];

                        spdlog::info("Variable k{} ({}) initialized to f: {} i: {}", varName, i, *(float*)&pVariableSet->data[i],
                                     *(int32_t*)&pVariableSet->data[i]);
                    }
                    else if (iter->second != pVariableSet->data[i])
                    {
                        const auto varName = s_varMap[i];

                        float floatCast = *(float*)&pVariableSet->data[i];
                        int intCast = *(int32_t*)&pVariableSet->data[i];
                        spdlog::warn("Variable k{} ({}) changed to f: {} i: {}", varName, i, floatCast,
                                     intCast);

                        s_values[i] = pVariableSet->data[i];
                        s_reusedValues[i] = pVariableSet->data[i];

                        char varTypeChar = varName[0]; //for guessing type, to see if u can find type char (i, f, b)
                        if (varTypeChar == 'f')
                        {
                            s_valueTypes[i] = 1;
                        }
                        else if (varTypeChar == 'i' && varName[1] != 's')
                        {
                            s_valueTypes[i] = 2;
                        }
                        else if (varTypeChar != 'b' && s_valueTypes[i] != 1) // no char hint to go off of and not assuming float
                        {
                            if (intCast > 1000 || intCast < -1000) // arbitrary int threshold
                            {
                                s_valueTypes[i] = 1; // assume float
                            }
                            else
                            {
                                if (intCast > 1 || intCast < 0)
                                {
                                    s_valueTypes[i] = 2; // assume int
                                }
                            }
                        }
                    }
                }
            }

            if (ImGui::Button("Dump variables") && pVariableSet)
            {
                //kinda ugly to iterate 3 times but idc cuz its just for debugging and its a small collection
                // BOOLS
                std::cout << "{" << std::endl;
                for (auto& [key, value] : s_reusedValues)
                {
                    if (s_valueTypes[key] == 0)
                    {
                        const auto varName = s_varMap[key];
                        std::cout << "k" << varName << "," << std::endl;
                    }
                }
                // FLOATS
                std::cout << "}," << std::endl
                          << "{" << std::endl;
                for (auto& [key, value] : s_reusedValues)
                {
                    if (s_valueTypes[key] == 1)
                    {
                        const auto varName = s_varMap[key];
                        std::cout << "k" << varName << "," << std::endl;
                    }
                }
                // INTS
                std::cout << "}," << std::endl
                          << "{" << std::endl;
                for (auto& [key, value] : s_reusedValues)
                {
                    if (s_valueTypes[key] == 2)
                    {
                        const auto varName = s_varMap[key];
                        std::cout << "k" << varName << "," << std::endl;
                    }
                }
                std::cout << "}" << std::endl;
            }

            if (ImGui::Button("Reset recording"))
            {
                s_values.clear();
                s_reusedValues.clear();
                spdlog::info("Variable recording has been reset");
            }

            ImGui::EndTabItem();
        }
    }

    ImGui::End();
}

void TestService::OnDraw() noexcept
{
    const auto view = m_world.view<FormIdComponent>();
    if (view.empty())
        return;

    AnimationDebugging();

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

#if TP_SKYRIM64
        auto* leftHandCaster = pPlayer->GetMagicCaster(MagicSystem::CastingSource::LEFT_HAND);
        auto* rightHandCaster = pPlayer->GetMagicCaster(MagicSystem::CastingSource::RIGHT_HAND);
        auto* otherHandCaster = pPlayer->GetMagicCaster(MagicSystem::CastingSource::OTHER);
        auto* instantHandCaster = pPlayer->GetMagicCaster(MagicSystem::CastingSource::INSTANT);

        ImGui::InputScalar("leftHandCaster", ImGuiDataType_U64, (void*)&leftHandCaster, 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("rightHandCaster", ImGuiDataType_U64, (void*)&rightHandCaster, 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("otherHandCaster", ImGuiDataType_U64, (void*)&otherHandCaster, 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("instantHandCaster", ImGuiDataType_U64, (void*)&instantHandCaster, 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);


        ImGui::InputScalar("leftHandCasterSpell", ImGuiDataType_U64, (void*)&(leftHandCaster->pCurrentSpell), 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("rightHandCasterSpell", ImGuiDataType_U64, (void*)&(rightHandCaster->pCurrentSpell), 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("otherHandCasterSpell", ImGuiDataType_U64, (void*)&(otherHandCaster->pCurrentSpell), 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("instantHandCasterSpell", ImGuiDataType_U64, (void*)&(instantHandCaster->pCurrentSpell), 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
#endif

#if TP_SKYRIM64
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
    ImGui::Begin("Form lookup");

    static uint32_t fetchFormId = 0;
    static TESForm* pFetchForm = nullptr;

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, &fetchFormId, 0, 0, "%" PRIx32, ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Look up"))
    {
        if (fetchFormId)
        {
            pFetchForm = TESForm::GetById(fetchFormId);
        }
    }

    if (pFetchForm)
    {
        ImGui::InputScalar("Memory address", ImGuiDataType_U64, (void*)&pFetchForm, 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("Vtable address", ImGuiDataType_U64, (void*)pFetchForm, 0, 0, "%" PRIx64,
                           ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);

        ImGui::InputInt("Form id", (int*)&pFetchForm->formID, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

        FormType formType = pFetchForm->GetFormType();
        ImGui::InputScalar("Form type", ImGuiDataType_U8, (void*)&formType, 0, 0, nullptr, ImGuiInputTextFlags_ReadOnly);
    }

    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Actor lookup");

    static uint32_t fetchActorId = 0;
    static Actor* pFetchActor = nullptr;

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, &fetchActorId, 0, 0, "%" PRIx32, ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Look up"))
    {
        if (fetchActorId)
        {
            auto* pFetchForm = TESForm::GetById(fetchActorId);
            if (pFetchForm)
                pFetchActor = RTTI_CAST(pFetchForm, TESForm, Actor);
            if (pFetchActor)
            {
                auto pExtension = pFetchActor->GetExtension();
                if (pExtension->IsLocal())
                    spdlog::warn("IsLocal");
                if (pExtension->IsRemote())
                    spdlog::warn("IsRemote");
                if (pExtension->IsLocalPlayer())
                    spdlog::warn("IsLocalPlayer");
                if (pExtension->IsRemotePlayer())
                    spdlog::warn("IsRemotePlayer");
            }
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

        int isRemote = int(pFetchActor->GetExtension()->IsRemote());
        ImGui::InputInt("Is remote?", &isRemote, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        int isPlayer = int(pFetchActor->GetExtension()->IsPlayer());
        ImGui::InputInt("Is player?", &isPlayer, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

    #if TP_SKYRIM64
        if (pFetchActor->currentProcess->middleProcess->ammoEquippedObject)
        {
            auto* pAmmo = pFetchActor->currentProcess->middleProcess->ammoEquippedObject->pObject;
            if (pAmmo)
            {
                auto ammoFormId = pAmmo->formID;
                ImGui::InputScalar("Ammo form Id", ImGuiDataType_U32, (void*)&ammoFormId, nullptr, nullptr, "%" PRIx32,
                                   ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
            }
        }

        if (ImGui::Button("Apply active effect"))
        {
            auto pEffectSpell = (MagicItem*)TESForm::GetById(0x5AD5F);
            auto activeEffects = pEffectSpell->listOfEffects;
            for (auto effect : activeEffects)
            {
                MagicTarget::AddTargetData data{};

                //data.pCaster = PlayerCharacter::Get();
                data.pSpell = pEffectSpell;
                data.pEffectItem = effect;
                data.pSource = nullptr;
                /*
                data.ExplosionLocation.x = -11891.820f;
                data.ExplosionLocation.y = -56157.960f;
                data.ExplosionLocation.z = 665.110f;
                */
                data.fMagnitude = 0.0f;
                data.fUnkFloat1 = 1.0f;
                data.eCastingSource = MagicSystem::CastingSource::CASTING_SOURCE_COUNT;
                /*
                data.bAreaTarget = true;
                data.bDualCast = false;
                */

                pFetchActor->magicTarget.AddTarget(data);
            }
        }
    #endif
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

#if TP_SKYRIM64
    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Container debug");

    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("ExtraContainerChanges"))
        {
            static uint32_t s_selectedInvFormId = 0;

            auto* pContainerActor = PlayerCharacter::Get();

            const auto pContainerChanges = pContainerActor->GetContainerChanges()->entries;

            ImGui::BeginChild("Items", ImVec2(0, 200), true);

            int i = 0;
            for (auto pChange : *pContainerChanges)
            {
                if (pChange && pChange->form && pChange->dataList)
                {
                    char name[256];
                    sprintf_s(name, std::size(name), "%x", pChange->form->formID);

                    if (ImGui::Selectable(name, s_selectedInvFormId == pChange->form->formID))
                    {
                        s_selectedInvFormId = pChange->form->formID;
                    }
                }
            }

            ImGui::EndChild();

            int itemCount = 0;
            int dataListCount = 0;

            ImVec4 red{255.f, 0.f, 0.f, 255.f};
            ImVec4 green{0.f, 255.f, 0.f, 255.f};

            for (auto pChange : *pContainerChanges)
            {
                if (pChange && pChange->form && pChange->form->formID == s_selectedInvFormId)
                {
                    itemCount++;

                    const auto pDataLists = pChange->dataList;
                    for (auto* pDataList : *pDataLists)
                    {
                        if (pDataList)
                        {
                            dataListCount++;

                            char name[256];
                            sprintf_s(name, std::size(name), "Item %d", itemCount);

                            if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
                            {
                                BSScopedLock<BSRecursiveLock> _(pDataList->lock);

                                bool charge = pDataList->Contains(ExtraData::Charge);
                                ImGui::TextColored(charge ? green : red, "charge");
                                if (charge)
                                {
                                    auto pCharge = (ExtraCharge*)pDataList->GetByType(ExtraData::Charge);
                                    ImGui::InputFloat("Charge", &pCharge->fCharge, 0, 0, "%.3f",
                                                      ImGuiInputTextFlags_ReadOnly);
                                }

                                bool count = pDataList->Contains(ExtraData::Count);
                                ImGui::TextColored(count ? green : red, "count");
                                if (count)
                                {
                                    auto pCount = (ExtraCount*)pDataList->GetByType(ExtraData::Count);
                                    auto iCount = int(pCount->count);
                                    ImGui::InputInt("Item count", &iCount, 0, 0, ImGuiInputTextFlags_ReadOnly);
                                }

                                bool enchantment = pDataList->Contains(ExtraData::Enchantment);
                                ImGui::TextColored(enchantment ? green : red, "enchantment");
                                if (enchantment)
                                {
                                    auto pEnchantment = (ExtraEnchantment*)pDataList->GetByType(ExtraData::Enchantment);
                                    int enchantmentID =
                                        pEnchantment->pEnchantment ? int(pEnchantment->pEnchantment->formID) : 0;
                                    ImGui::InputInt("Enchantment id", &enchantmentID, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
                                    int iCharge = int(pEnchantment->usCharge);
                                    ImGui::InputInt("Charge", &iCharge, 0, 0, ImGuiInputTextFlags_ReadOnly);
                                    int iRemoveOnUnequip = int(pEnchantment->bRemoveOnUnequip);
                                    ImGui::InputInt("Remove on unequip?", &iRemoveOnUnequip, 0, 0, ImGuiInputTextFlags_ReadOnly);
                                }

                                bool health = pDataList->Contains(ExtraData::Health);
                                ImGui::TextColored(health ? green : red, "health");
                                if (health)
                                {
                                    auto pHealth = (ExtraHealth*)pDataList->GetByType(ExtraData::Health);
                                    ImGui::InputFloat("Health", &pHealth->fHealth, 0, 0, "%.3f",
                                                      ImGuiInputTextFlags_ReadOnly);
                                }

                                bool poison = pDataList->Contains(ExtraData::Poison);
                                ImGui::TextColored(poison ? green : red, "poison");
                                if (poison)
                                {
                                    auto pPoison = (ExtraPoison*)pDataList->GetByType(ExtraData::Poison);
                                    int poisonID =
                                        pPoison->pPoison ? int(pPoison->pPoison->formID) : 0;
                                    ImGui::InputInt("Poison id", &poisonID, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
                                    int iCount = int(pPoison->uiCount);
                                    ImGui::InputInt("Count", &iCount, 0, 0, ImGuiInputTextFlags_ReadOnly);
                                }

                                bool soul = pDataList->Contains(ExtraData::Soul);
                                ImGui::TextColored(soul ? green : red, "soul");
                                if (soul)
                                {
                                    auto pSoul = (ExtraSoul*)pDataList->GetByType(ExtraData::Soul);
                                    auto iSoulLevel = int(pSoul->cSoul);
                                    ImGui::InputInt("Soul level", &iSoulLevel, 0, 0, ImGuiInputTextFlags_ReadOnly);
                                }

                                bool textDisplayData = pDataList->Contains(ExtraData::TextDisplayData);
                                ImGui::TextColored(textDisplayData ? green : red, "textDisplayData");
                                if (textDisplayData)
                                {
                                    auto pTextDisplayData = (ExtraTextDisplayData*)pDataList->GetByType(ExtraData::TextDisplayData);
                                    char name[256];
                                    sprintf_s(name, std::size(name), "%s", pTextDisplayData->DisplayName.AsAscii());
                                    ImGui::InputText("Name", name, std::size(name), ImGuiInputTextFlags_ReadOnly);
                                }

                                bool worn = pDataList->Contains(ExtraData::Worn);
                                ImGui::TextColored(worn ? green : red, "worn");
                                bool wornLeft = pDataList->Contains(ExtraData::WornLeft);
                                ImGui::TextColored(wornLeft ? green : red, "wornLeft");
                            }
                        }
                    }
                }
            }

            if (ImGui::CollapsingHeader("Metadata", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::InputInt("Item count", &itemCount, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
                ImGui::InputInt("Data list count", &dataListCount, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("TESContainer"))
        {
            static uint32_t s_selectedInvFormId = 0;

            PlayerCharacter* pPlayer = PlayerCharacter::Get();
            TESContainer* pContainer = pPlayer->GetContainer();
            if (pContainer && pContainer->entries)
            {
                ImGui::Text("Items (%d)", pContainer->count);

                ImGui::BeginChild("Items", ImVec2(0, 200), true);

                for (int i = 0; i < pContainer->count; ++i)
                {
                    auto pEntry = pContainer->entries[i];
                    if (!pEntry || !pEntry->form)
                        continue;

                    char name[256];
                    sprintf_s(name, std::size(name), "%x (%d)", pEntry->form->formID, pEntry->count);

                    if (ImGui::Selectable(name, s_selectedInvFormId == pEntry->form->formID))
                    {
                        s_selectedInvFormId = pEntry->form->formID;
                    }
                }

                ImGui::EndChild();
            }

            ImGui::EndTabItem();
        }
    }


    ImGui::End();
#endif
}

