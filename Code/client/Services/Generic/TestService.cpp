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

#include <Components.h>
#include <World.h>

#include <Games/TES.h>
#include <Forms/TESWorldSpace.h>
#include <Forms/TESObjectCELL.h>

#include <imgui.h>
#include <inttypes.h>

enum Flags
{
    kSpeed = 0,
    kDirection = 1,
    kTurnDelta = 2,
    kiSyncIdleLocomotion = 3,
    kSpeedWalk = 4,
    kSpeedRun = 5,
    kfSpeedMin = 6,
    kuseShield = 7,
    kuseMLh = 8,
    kblendMoveStart = 9,
    kblendMoveStop = 10,
    kblendDefault = 11,
    kWalkSpeedMult = 12,
    kTurnDeltaDamped = 13,
    kTurnMin = 14,
    kIsFirstPerson = 15,
    kPitch = 16,
    kPitchLook = 17,
    kblendAttackPowerFast = 18,
    kblendAttackPowerSlow = 19,
    kblendFast = 20,
    kIsPlayer = 21,
    kattackPowerStartTime = 22,
    kblendMove1stP = 23,
    kIntVariable = 24,
    kIsNPC = 25,
    kblendSlow = 26,
    kPitchDefault = 27,
    kPitchOverride = 28,
    kstaggerMagnitude = 29,
    kblockDown = 30,
    kblockLeft = 31,
    kblockRight = 32,
    kblockUp = 33,
    krecoilMagnitude = 34,
    kiRightHandType = 35,
    kIsBlocking = 36,
    kiWantBlock = 37,
    kIsAttackReady = 38,
    kiAnnotation = 39,
    kSpeedSampled = 40,
    kbMotionDriven = 41,
    kblendAttackRelease = 42,
    kattackComboStartFraction = 43,
    kblendAttackCombo = 44,
    kattackIntroLength = 45,
    kIsShouting = 46,
    kweaponSpeedMult = 47,
    kiSyncTurnState = 48,
    ki1stPerson = 49,
    kIsSprinting = 50,
    kbIsSynced = 51,
    kIsBlockHit = 52,
    kTimeDelta = 53,
    kPitchOffset = 54,
    kPitchAcc = 55,
    kPitchThresh = 56,
    kUsePitch = 57,
    kDisablePitch = 58,
    kweapAdj = 59,
    kiLeftHandType = 60,
    kweapChangeStartFraction = 61,
    k1stPRot = 62,
    k1stPRotDamped = 63,
    kRotMax = 64,
    kPitchManualOverride = 65,
    kSpeedAcc = 66,
    kIsCastingRight = 67,
    kIsCastingLeft = 68,
    kCastBlend = 69,
    kbVoiceReady = 70,
    kbWantCastLeft = 71,
    kbWantCastRight = 72,
    kbWantCastVoice = 73,
    kBlendDamped = 74,
    kb1HM_MLh_attack = 75,
    kIs1HM = 76,
    ki1HMState = 77,
    kRawBlend = 78,
    kBlendAcc = 79,
    kb1HMCombat = 80,
    kbAnimationDriven = 81,
    kbCastReady = 82,
    kIsStaggering = 83,
    kIsRecoiling = 84,
    kIsAttacking = 85,
    kIsInCastState = 86,
    kbleedoutTurnMult = 87,
    kIsInCastStateDamped = 88,
    kbAllowRotation = 89,
    kbMagicDraw = 90,
    kCastBlendDamped = 91,
    kbMLh_Ready = 92,
    kbMRh_Ready = 93,
    kiState = 94,
    kiState_NPCSprinting = 95,
    kiState_NPCDefault = 96,
    kfIsFirstPerson = 97,
    kbInMoveState = 98,
    kiState_NPCSneaking = 99,
    kiState_NPCBowDrawn = 100,
    kBoolVariable = 101,
    kDualMagicState = 102,
    kiDualMagicState = 103,
    kInDualMagicState = 104,
    kiState_NPCBlocking = 105,
    kiState_NPCBleedout = 106,
    kiBlockState = 107,
    kbSprintOK = 108,
    kiSyncSprintState = 109,
    kIsEquipping = 110,
    kIsUnequipping = 111,
    kiIsInSneak = 112,
    kcurrentDefaultState = 113,
    kBowZoom = 114,
    kbowZoomAmt = 115,
    kbIdlePlaying = 116,
    kiMagicEquipped = 117,
    kiEquippedItemState = 118,
    kiMagicState = 119,
    kIsCastingDual = 120,
    kbIsDialogueExpressive = 121,
    kiIsDialogueExpressive = 122,
    kbAnimObjectLoaded = 123,
    kfEquipWeapAdj = 124,
    kbEquipUnequip = 125,
    kbAttached = 126,
    kbodyMorphWeight = 127,
    kIsBashing = 128,
    kbEquipOk = 129,
    kIsBleedingOut = 130,
    kfMagicForceEquipBlendValue = 131,
    kFemaleOffset = 132,
    kblendVerySlow = 133,
    kblendDialogue = 134,
    kiSyncIdleState = 135,
    kfIdleTimer = 136,
    kbIsH2HSolo = 137,
    kAdditiveHeadTransl = 138,
    kAdditiveHeadScale = 139,
    kAdditiveHeadRot = 140,
    k2HMweaponSpeedMult = 141,
    kiState_NPC1HM = 142,
    kiState_NPC2HM = 143,
    kiState_NPCBow = 144,
    kiState_NPCMagic = 145,
    kiState_NPCMagicCasting = 146,
    kiState_NPCHorse = 147,
    kiState_HorseSprint = 148,
    kiCharacterSelector = 149,
    krandomStartTime = 150,
    kbHeadTracking = 151,
    kbIsRiding = 152,
    kfMinSpeed = 153,
    kiCombatStance = 154,
    kturnSpeedMult = 155,
    kiSyncTurnDirection = 156,
    kfTwistDirection = 157,
    kIntDirection = 158,
    kDirectionRange = 159,
    kDirectionRangeDamped = 160,
    kDirectionDamped = 161,
    kIsDismounting = 162,
    kTargetLocation = 163,
    kbTalkable = 164,
    kbNeutralState = 165,
    kCastOK = 166,
    kblendFromAnimDriven = 167,
    kwalkBackRate = 168,
    kbRitualSpellActive = 169,
    kSwimDirection = 170,
    kiRegularAttack = 171,
    kIsAttackReady_32 = 172,
    kcamerafromx = 173,
    kcamerafromy = 174,
    kcamerafromz = 175,
    kVelocityZ = 176,
    kbInJumpState = 177,
    kLookAtOutOfRange = 178,
    kiRightHandEquipped = 179,
    kiLeftHandEquipped = 180,
    ktestint = 181,
    kiIsPlayer = 182,
    kSpeedDamped = 183,
    kbodyMorphMuscular = 184,
    kbHeadTrackSpine = 185,
    kIsSneaking = 186,
    kiGetUpType = 187,
    kiState_NPCAttacking = 188,
    kiState_NPCPowerAttacking = 189,
    kiState_NPCAttacking2H = 190,
    kLookAtOnGain = 191,
    kLookAtOffGain = 192,
    kLookAtEyeOnGain = 193,
    kLookAtEyeOffGain = 194,
    kiDrunkVariable = 195,
    kiState_NPCDrunk = 196,
    kleftWeaponSpeedMult = 197,
    kbLeftHandAttack = 198,
    kBlendJump = 199,
    kbIsInMT = 200,
    kBoolVariable00 = 201,
    kbHumanoidFootIKEnable = 202,
    ktestPitchOffset = 203,
    kbBowDrawn = 204,
    kIsBeastRace = 205,
    kbHumanoidFootIKDisable = 206,
    kiTempSwitch = 207,
    kiState_NPCBowDrawnQuickShot = 208,
    kiState_NPCBlockingShieldCharge = 209,
    kbStaggerPlayerOverride = 210,
    kbNoStagger = 211,
    kbIsStaffLeftCasting = 212,
    kIs3rdPKillOnly = 213,
    kbPerkShieldCharge = 214,
    kbPerkQuickShot = 215,
    km_bEnablePitchTwistModifier = 216,
    km_worldFromModelFeedbackGain = 217,
    kIsBleedingOutTransition = 218,
    kbMesmerize = 219,
    kiIsHorse = 220,
    kbWantMountedWeaponAnims = 221,
    kiUseMountedSwimming = 222,
    kAimPitchCurrent = 223,
    kAimHeadingCurrent = 224,
    kbAimActive = 225,
    kAimGainOff = 226,
    kAimGainOnMounted = 227,
    kAimGainOnDragonMount = 228,
    kiWantMountedWeaponAnims = 229,
    kiWeaponReady = 230,
    kZoomAcc = 231,
    kAimHeadingMax = 232,
    kAimPitchMax = 233,
    kAimGainOn = 234,
    kbDisableInterp = 235,
    kBowAimOffsetPitch = 236,
    kBowAimOffsetHeading = 237,
    kbPerkQuickDraw = 238,
    kbForceIdleStop = 239,
    kIsInDWattack = 240,
    kbShortKillMove = 241,
    kbLongKillMove = 242,
    kiCrossbowState = 243,
    kInt32Variable = 244,
    kAllowBlockModify = 245,
    k_iState_NPCSneaking = 246,
    kMagicLeftActive = 247,
    kAllowMagicModify = 248,
    kMagicAimOffsetHeading = 249,
    kMagicAimOffsetPitch = 250,
    kNotCasting = 251,
    kCastBlendGain = 252,
    kiWantBlock01 = 253,
    kstaggerDirection = 254,
    kiSyncForwardState = 255,
    kfMinTurnDelta = 256,
    kbHeadTrackingOn = 257,
    km_onOffGain = 258,
    km_groundAscendingGain = 259,
    km_groundDescendingGain = 260,
    km_footPlantedGain = 261,
    km_footRaisedGain = 262,
    km_footUnlockGain = 263,
    km_errorUpDownBias = 264,
    km_alignWorldFromModelGain = 265,
    km_hipOrientationGain = 266,
    km_footPlantedAnkleHeightMS = 267,
    km_footRaisedAnkleHeightMS = 268,
    km_maxAnkleHeightMS = 269,
    km_minAnkleHeightMS = 270,
    kbDisableHeadTrack = 271,
    kbCanHeadTrack = 272,
    kisIdleSitting = 273,
    kbMirroredAttack = 274,
    kfMinMoveSpeed = 275,
    kiState_HorseDefault = 276,
    kHorseSpeedSampled = 277,
    kwalkBackSpeedMult = 278,
    kiState_HorseFall = 279,
    kbInSwimState = 280,
    kiState_HorseSwim = 281,
    kbHorseFootIKEnable = 282,
    kbNoHeadTrack = 283,
    kIsMoving = 284,
    kblendVeryFast = 285,
    kAimHeadingMaxMounted = 286,
    kAimPitchMaxMounted = 287,
    kMC_2HM_WeapSpeed = 288,
    kiSyncDialogueResponse = 289,
    kbBlendOutSlow = 290,
    kisInFurniture = 291,
    kbIdleBeforeConversation = 292,
    kbMotionDrivenDialogue = 293,
    kbAnimationDrivenDialogue = 294,
    kbHDDialogue = 295,
    kbHHDialogue = 296,
    kbTalkableWithItem = 297,
    kisCarryItem = 298,
    kIsRomanticFollower = 299,
    kIsRF_Talk = 300,
};

enum Actions
{
    kRightAttack = 0x13005,
    kJump = 0x13006,
    kLand = 0x937F5,
    kDraw = 0x132AF,
    kRightPowerAttack = 0x13383,
    kRightRelease = 0x13454,
    kActivate = 0x13009,
    kMoveStart = 0x959F8,
    kMoveStop = 0x959F9,
    kFall = 0x937F4,
    kSheath = 0x46BAF,
    kTurnRight = 0x959FC,
    kTurnLeft = 0x959FD,
    kTurnStop = 0x959FE
};

/*
enum FloatVariables
{
    kSpeed = 0,
    kDirection = 1,
    kSpeedSampled = 40,
    kweapAdj = 59,
    kFloatCount = 4
};

enum IntegerVariables
{
    kTurnDelta = 2,
    kiRightHandEquipped = 179,
    kiLeftHandEquipped = 180,
    i1HMState = 77,
    kiState = 94,
    kiLeftHandType = 60,
    kiRightHandType = 35,
    kIntegerCount = 8
};
*/

static AnimationGraphDescriptorManager::Builder s_builder("Master_Behavior", AnimationGraphDescriptor{
        {129, 41,  205, 186, 130, 120, 76,  67,  68,  52,  21,  25,  51,  70,  71,  72,
                                 73,  75,  80,  81,  82,  89,  90,  92,  93,  98,  108, 116, 121, 123, 125, 126,
                                 137, 151, 152, 164, 169, 177, 185, 198, 200, 202, 206, 210, 211, 212, 214, 215,
                                 85,  36,  128, 83,  84,  110, 111, 291, 165, 3,   255, 48,  112, 37,  171, 204,
                                 kiMagicEquipped, kNotCasting, kPitchOverride},
        {kDirection, kSpeedSampled, kweapAdj, kSpeed, kCastBlendDamped, kCastBlend, kSpeedDamped},
        {kTurnDelta, kiRightHandEquipped, kiLeftHandEquipped, ki1HMState, kiState, kiLeftHandType, kiRightHandType, ktestint, kcurrentDefaultState}
    });

extern thread_local bool g_overrideFormId;

void __declspec(noinline) TestService::PlaceActorInWorld() noexcept
{
    const auto pPlayerBaseForm = static_cast<TESNPC*>(PlayerCharacter::Get()->baseForm);

    //const auto pNpc = TESNPC::Create(data, pPlayerBaseForm->GetChangeFlags());
    auto pActor = Actor::Create(pPlayerBaseForm);

    pActor->SetInventory(PlayerCharacter::Get()->GetInventory());
    /*
    auto pExtendedActor = pActor->GetExtension();
    pExtendedActor->SetRemote(false);
    */

    m_actors.emplace_back(pActor);
}

TestService::TestService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport, ImguiService& aImguiService)
    : m_dispatcher(aDispatcher)
    , m_transport(aTransport)
    , m_world(aWorld)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&TestService::OnUpdate>(this);
    m_drawImGuiConnection = aImguiService.OnDraw.connect<&TestService::OnDraw>(this);
#if TP_SKYRIM64
    m_dispatcher.sink<MagicSyncEvent>().connect<&TestService::OnMagicSyncEvent>(this);
    m_dispatcher.sink<ActionEvent>().connect<&TestService::OnActionEvent>(this);
#endif
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
#if TP_SKYRIM64
    if (!m_actions.empty())
    {
        for (const auto action : m_actions)
        {
            //ExecuteAction(action);
        }

        m_actions.clear();
    }
#endif

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

#if TP_SKYRIM64
void TestService::OnActionEvent(const ActionEvent& acEvent) noexcept
{
    m_actions.push_back(acEvent);
}

void TestService::ExecuteAction(const ActionEvent& acEvent) noexcept
{
    if (m_actors.empty())
        return;

    auto pActor = m_actors[0];

    const auto actionId = acEvent.ActionId;
    const auto targetId = acEvent.TargetId;

    const auto pAction = RTTI_CAST(TESForm::GetById(actionId), TESForm, BGSAction);
    const auto pTarget = RTTI_CAST(TESForm::GetById(targetId), TESForm, TESObjectREFR);

    pActor->actorState.flags1 = acEvent.State1;
    pActor->actorState.flags2 = acEvent.State2;

    pActor->LoadAnimationVariables(acEvent.Variables);

    // Play the animation
    TESActionData actionData(acEvent.Type & 0x3, pActor, pAction, pTarget);
    actionData.eventName = BSFixedString(acEvent.EventName.c_str());
    actionData.idleForm = RTTI_CAST(TESForm::GetById(acEvent.IdleId), TESForm, TESIdleForm);
    actionData.someFlag = ((acEvent.Type & 0x4) != 0) ? 1 : 0;

    //if (actionData.action)
        //spdlog::info("Animation event: {}, action: {}, form id: {:X}", actionData.eventName.AsAscii(), actionData.action->keyword.AsAscii(), actionData.action->formID);

    const auto result = ActorMediator::Get()->ForceAction(&actionData);

    //spdlog::info("\t{}", result);
}

void SpellDraw(Actor* apActor)
{
    using TSpellDraw = void(void* garbage, Actor* apActor);

    POINTER_SKYRIMSE(TSpellDraw, s_spellDraw, 0x140720FB0 - 0x140000000);

    s_spellDraw.Get()(nullptr, apActor);
}

void WeaponDraw(Actor* apActor, bool aDraw)
{
    apActor->DrawWeapon(aDraw);
}

void TestService::OnMagicSyncEvent(const MagicSyncEvent& acEvent) noexcept
{
    //SpellDraw(acEvent.pActor);
    WeaponDraw(acEvent.pActor, acEvent.Flag);
}

void TestService::ControlTestActor() noexcept
{
    static uint32_t fetchFormId = 0;
    static Actor* pFetchActor = 0;

    ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiCond_FirstUseEver);
    ImGui::Begin("Control test actor");

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

    if (!pFetchActor)
    {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Set inventory"))
        pFetchActor->SetInventory(PlayerCharacter::Get()->GetInventory());

    if (ImGui::Button("Draw"))
        m_world.GetRunner().Trigger(MagicSyncEvent(pFetchActor, true));

    if (ImGui::Button("Sheath"))
        m_world.GetRunner().Trigger(MagicSyncEvent(pFetchActor, false));


    static Map<uint32_t, uint32_t> s_values;
    static Map<uint32_t, uint32_t> s_reusedValues;
    static Vector<uint32_t> s_blacklist{ kTimeDelta, kiSyncSprintState, kBlendJump, kPitchOffset, kPitch, kVelocityZ, kIsSprinting };

    BSAnimationGraphManager* pManager = nullptr;

    pFetchActor->animationGraphHolder.GetBSAnimationGraph(&pManager);

    if (pManager)
    {
        ImGui::InputInt("Animation graph count", (int*)&pManager->animationGraphs.size, 0, 0, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputInt("Animation graph index", (int*)&pManager->animationGraphIndex, 0, 0, ImGuiInputTextFlags_ReadOnly);

        const auto pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);
        if (pGraph)
        {
            char name[256];
            sprintf_s(name, std::size(name), "%s", pGraph->behaviorGraph->stateMachine->name);
            ImGui::InputText("Graph state machine name", name, std::size(name), ImGuiInputTextFlags_ReadOnly);

            const auto pVariableSet = pGraph->behaviorGraph->animationVariables;
            auto pDescriptor =
                AnimationGraphDescriptorManager::Get().GetDescriptor(pGraph->behaviorGraph->stateMachine->name);

            static bool toggleVariableCount = false;
            if (ImGui::Button("Toggle variable counting"))
            {
                toggleVariableCount = !toggleVariableCount;
                spdlog::info("Toggle variable count: {}", toggleVariableCount);
            }

            if (pDescriptor && pVariableSet && toggleVariableCount)
            {
                for (auto i = 0u; i < pVariableSet->size; ++i)
                {
                    if (std::find(s_blacklist.begin(), s_blacklist.end(), i) != s_blacklist.end())
                        continue;

                    auto itor = s_values.find(i);
                    if (itor == std::end(s_values) && !pDescriptor->IsSynced(i))
                    {
                        s_values[i] = pVariableSet->data[i];

                        spdlog::info("Variable {} initialized to f: {} i: {}", i, *(float*)&pVariableSet->data[i],
                                     *(int32_t*)&pVariableSet->data[i]);
                    }
                    else if (itor->second != pVariableSet->data[i] && !pDescriptor->IsSynced(i))
                    {
                        spdlog::warn("Variable {} changed to f: {} i: {}", i, *(float*)&pVariableSet->data[i],
                                     *(int32_t*)&pVariableSet->data[i]);

                        s_values[i] = pVariableSet->data[i];
                        s_reusedValues[i] = pVariableSet->data[i];
                    }
                }
            }

            if (ImGui::Button("Dump variables") && pVariableSet)
            {
                std::map<uint32_t, std::tuple<const char*, uint32_t>> varMap;
                pManager->DumpAnimationVariables(varMap);
                for (auto& [key, value] : s_reusedValues)
                {
                    const auto varKey = varMap[key];
                    const auto* varName = std::get<0>(varKey);
                    spdlog::warn("Variable: k{}, id: {}, f: {}, i: {}", varName, key, *(float*)&pVariableSet->data[key],
                                 *(int32_t*)&pVariableSet->data[key]);
                }
            }
        }
    }

    ImGui::End();
}
#endif

void TestService::AnimationDebugging() noexcept
{
}

void TestService::OnDraw() noexcept
{
    static uint32_t fetchFormId;
    static Actor* pFetchActor = 0;

    const auto view = m_world.view<FormIdComponent>();
    if (view.empty())
        return;

#if TP_SKYRIM64
    ControlTestActor();
    AnimationDebugging();
#endif

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


