#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Master_Behavior.h>

// First person variables
namespace FP
{
    enum Variables
    {
        kfSpeedWalk = 0,
        kfSpeedRun = 1,
        kSpeed = 2,
        kDirection = 3,
        kiSyncIdleLocomotion = 4,
        kCamPitchForward = 5,
        kCamPitchBackward = 6,
        kSpineTwist = 7,
        kCamRoll = 8,
        kiState = 9,
        kIsFirstPerson = 10,
        km_bEnablePitchTwistModifier = 11,
        kPitchOffset = 12,
        kiState_PlayerDefault = 13,
        kCamPitch = 14,
        kCamPitchDamped = 15,
        kCamRollDamped = 16,
        kdamper_kP = 17,
        kdamper_kI = 18,
        kdamper_kD = 19,
        kSpineTwistDamped = 20,
        kPitch = 21,
        kiSyncSprintState = 22,
        kIsSprinting = 23,
        kTurnDelta = 24,
        kfTest = 25,
        kfDampenTwist = 26,
        kspeedDamped = 27,
        kisFiring = 28,
        kfDampenSighted = 29,
        kfTest3 = 30,
        kisReloading = 31,
        kfTurnDeltaSpeedLimited = 32,
        kiState_NPCSighted = 33,
        kfTurnDeltaSpeedLimit = 34,
        kTurnDeltaSpeedLimitedDampened = 35,
        kiAttackState = 36,
        kIsAttackReady = 37,
        kPitchDelta = 38,
        kTurnDeltaSmoothed = 39,
        kPitchDeltaSmoothed = 40,
        kTurnDeltaDamped = 41,
        kDirectionDamped = 42,
        kPitchDeltaDamped = 43,
        kTurnDeltaSmoothedDamped = 44,
        kPitchDeltaSmoothedDamped = 45,
        kbAnimationDriven = 46,
        kisAttackNotReady = 47,
        kVelocityZ = 48,
        kDirectionSmoothed = 49,
        kAimStability = 50,
        kisJumping = 51,
        kisAttacking = 52,
        kweaponSpeedMult = 53,
        kiState_NPCSneaking = 54,
        kiZoomBehavior = 55,
        kfControllerYSum = 56,
        kiZoomState = 57,
        kbDisableLooking = 58,
        kbEnableLooking = 59,
        kfControllerXSmoothed = 60,
        kfControllerYSmoothed = 61,
        kfPitchFactor = 62,
        kfTurnFactor = 63,
        kbAllowRotation = 64,
        kfArmTurnFactor = 65,
        kfArmPitchFactor = 66,
        kfIdleSpeedMult = 67,
        kiCategory = 68,
        kfZoomLevel1 = 69,
        kTest = 70,
        kbAdjust1stPersonFOV = 71,
        kbIsSynced = 72,
        kbGraphDrivenRotation = 73,
        kbGraphDriven = 74,
        kbRenderFirstPersonInWorld = 75,
        kstaggerMagnitude = 76,
        kstaggerDirection = 77,
        kiState_NPCAttacking = 78,
        kbEquipOk = 79,
        kiState_PlayerMelee = 80,
        kiIsInSneak = 81,
        kiState_NPCGun = 82,
        kiState_Player1HM = 83,
        kiState_Player2HM = 84,
        kiState_PlayerPistol = 85,
        kiState_PlayerShoulderMounted = 86,
        kiState_PlayerBigGuns = 87,
        kiState_NPCFastWalk_1stP = 88,
        kiState_PlayerSneakMelee = 89,
        kSpeedSmoothed = 90,
        kfControllerXRaw = 91,
        kfControllerYRaw = 92,
        kiSyncChargeState = 93,
        kReloadSpeedMult = 94,
        kAimWobble = 95,
        kiSyncSightedState = 96,
        kiSyncWeaponDrawState = 97,
        kisStagger = 98,
        kbBlockPOVSwitch = 99,
        kbIsThrowing = 100,
        kbBlockPipboy = 101,
        kFirstPersonSpeedSmoothed = 102,
        kLeftHandTarget = 103,
        kiWeaponChargeMode = 104,
        kiSyncGunDown = 105,
        kSightedSpeedMult = 106,
        kAimWobbleSpeedMult = 107,
        kiSyncJumpState = 108,
        kfCrippledWobbleWeight = 109,
        kbDoNotAllowMelee = 110,
        kbDoNotAllowThrow = 111,
        kbIsAnimationDriven = 112,
        kiWantBlock = 113,
        kIsBlocking = 114,
        kbGraphWantsFootIK = 115,
        kbAllowHeadTracking = 116,
        kLookAtOutOfRange = 117,
        kcamerafromx = 118,
        kcamerafromy = 119,
        kcamerafromz = 120,
        kfRandomClipStartTimePercentage = 121,
        kLookAtChest_Enabled = 122,
        kLookAtSpine2_Enabled = 123,
        kbFreezeSpeedUpdate = 124,
        kbDisableSpineTracking = 125,
        kIsPlayer = 126,
        kPose = 127,
        kbManualGraphChange = 128,
        kpipboyUp = 129,
        kfControllerXSum = 130,
        kbAimActive = 131,
        kAimHeadingCurrent = 132,
        kAimPitchCurrent = 133,
        kAimHeadingMaxCCW = 134,
        kAimHeadingMaxCW = 135,
        kfDirectAtSavedGain = 136,
        kAimPitchMaxUp = 137,
        kAimPitchMaxDown = 138,
        kIsNPC = 139,
        kfPACameraAdd = 140,
        kfPipboyInputYScaled = 141,
        kfPipboyInputXScaled = 142,
        kpipboyState = 143,
        kfPipboyInputX = 144,
        kfPipboyInputY = 145,
        kfPipboyInputMagnitude = 146,
        ktest_holotape = 147,
        kfIdleTimer = 148,
        kfRadioTune = 149,
        kfZeroToOneAngle = 150,
        kRH_HandIKTarget_WeaponLeft = 151,
        ktestFloat = 152,
        ktestPipboyIK = 153,
        ktestModCamPitch = 154,
        kiPipboyRadio = 155,
        ktestRadioBlend = 156,
        kfZoom2 = 157,
        kfZoom1 = 158,
        kfZoom0 = 159,
        kfRadioZoom1 = 160,
        kfRadioZoom2 = 161,
        kiFromRadio = 162,
        kiPipboyRootStartState = 163,
        kLookAtOutsideLimit = 164,
        kbForceIdleStop = 165,
        kbDoNotInterrupt = 166,
        kLookAtLimitAngleDeg = 167,
        kLookAtChest_LimitAngleDeg = 168,
        kLookAtNeck_LimitAngleDeg = 169,
        kLookAtHead_LimitAngleDeg = 170,
        kLookAtHead_OnGain = 171,
        kLookAtAdditive = 172,
        kbEnableRoot_IsActiveMod = 173,
        kLookAtOnGain = 174,
        kLookAtOffGain = 175,
        kbTalkableWithItem = 176,
        kiTalkGenerator = 177,
        kbFreezeRotationUpdate = 178,
        kiPcapTalkGenerator = 179,
        kbIsInFlavor = 180,
    };
}

// Third person variables
namespace TP
{
    enum Variables
    {
        kfSpeedWalk = 0,
        kfSpeedRun = 1,
        kDirection = 2,
        kiSyncIdleLocomotion = 3,
        kCamPitchForward = 4,
        kCamPitchBackward = 5,
        kSpineTwist = 6,
        kCamRoll = 7,
        kiState = 8,
        kIsFirstPerson = 9,
        km_bEnablePitchTwistModifier = 10,
        kPitchOffset = 11,
        kiState_NPCDefault = 12,
        kCamPitch = 13,
        kCamPitchDamped = 14,
        kCamRollDamped = 15,
        kdamper_kP = 16,
        kdamper_kI = 17,
        kdamper_kD = 18,
        kSpineTwistDamped = 19,
        kTest = 20,
        kPitch = 21,
        kiSyncSprintState = 22,
        kIsSprinting = 23,
        kTurnDelta = 24,
        kfTEST = 25,
        kfDampenTwist = 26,
        kspeedDamped = 27,
        kisFiring = 28,
        kfDampenSighted = 29,
        kfTest3 = 30,
        kisReloading = 31,
        kfTurnDeltaSpeedLimited = 32,
        kiState_NPCSighted = 33,
        kfTurnDeltaSpeedLimit = 34,
        kTurnDeltaSpeedLimitedDampened = 35,
        kiAttackState = 36,
        kIsAttackReady = 37,
        kPitchDelta = 38,
        kTurnDeltaSmoothed = 39,
        kPitchDeltaSmoothed = 40,
        kTurnDeltaDamped = 41,
        kDirectionDamped = 42,
        kPitchDeltaDamped = 43,
        kTurnDeltaSmoothedDamped = 44,
        kPitchDeltaSmoothedDamped = 45,
        kbAnimationDriven = 46,
        kisAttackNotReady = 47,
        kVelocityZ = 48,
        kDirectionSmoothed = 49,
        kAimStability = 50,
        kiIsInSneak = 51,
        kisJumping = 52,
        kbAllowRotation = 53,
        kSpeed = 54,
        kbEquipOk = 55,
        kforceDirectionVector = 56,
        kcamerafromx = 57,
        kcamerafromy = 58,
        kcamerafromz = 59,
        kLookAtOutsideLimit = 60,
        kAimHeadingMax = 61,
        kAimPitchMax = 62,
        kBowAimOffsetHeading = 63,
        kBowAimOffsetPitch = 64,
        kbAimActive = 65,
        kAimHeadingCurrent = 66,
        kAimPitchCurrent = 67,
        kIsNPC = 68,
        kbHeadTrackingOn = 69,
        kbHeadTrackingOff = 70,
        kSampledSpeed = 71,
        kiSyncTurnState = 72,
        kisLevitating = 73,
        kbFailMoveStart = 74,
        kbIsSynced = 75,
        kbDelayMoveStart = 76,
        kbVoiceReady = 77,
        kFootIKDisable = 78,
        kbInJumpState = 79,
        kbWantCastVoice = 80,
        kIsStaggering = 81,
        kstaggerMagnitude = 82,
        kiGetUpType = 83,
        kiSyncFireState = 84,
        kIsSneaking = 85,
        kisMirrored = 86,
        kiSyncRunDirection = 87,
        kfTimeStep = 88,
        kSpineXTwist = 89,
        kSpineYTwist = 90,
        kSpineZTwist = 91,
        kfSpineTwistGain = 92,
        kfSpineTwistGainAdj = 93,
        kHeadZTwist = 94,
        kHeadYTwist = 95,
        kHeadXTwist = 96,
        kfHeadTwistGain = 97,
        kfHeadTwistGainAdj = 98,
        kfStumbleTimerThreshold = 99,
        kfStumbleSpeedStopThreshold = 100,
        kfStumbleDir = 101,
        kfStumbleDirDeltaStopThreshold = 102,
        kcHitReactionDir = 103,
        kcHitReactionBodyPart = 104,
        kiState_Raider_Stumble_Rifle = 105,
        kbNotHeadTrack = 106,
        kbShouldAimHeadTrack = 107,
        kbSupportedDeathAnim = 108,
        kbCCSupport = 109,
        kbCCOnStairs = 110,
        kfMaxForce = 111,
        kbAllowHeadTracking = 112,
        kbGraphDriven = 113,
        kbGraphDrivenTranslation = 114,
        kbGraphDrivenRotation = 115,
        kbGraphMotionIsAdditive = 116,
        kbShouldBeDrawn = 117,
        kiState_NPCSneaking = 118,
        kPose = 119,
        kLookAtSpine2_LimitAngleDeg = 120,
        kLookAtSpine2_OnGain = 121,
        kLookAtSpine2_OffGain = 122,
        kLookAtSpine2_OnLeadIn = 123,
        kLookAtSpine2_OffLeadIn = 124,
        kLookAtSpine2_Enabled = 125,
        kLookAtSpine2_FwdAxisLS = 126,
        kLookAtChest_FwdAxisLS = 127,
        kLookAtChest_LimitAngleDeg = 128,
        kLookAtChest_OnGain = 129,
        kLookAtChest_OffGain = 130,
        kLookAtChest_OnLeadIn = 131,
        kLookAtChest_OffLeadIn = 132,
        kLookAtChest_Enabled = 133,
        kLookAtNeck_FwdAxisLS = 134,
        kLookAtNeck_LimitAngleDeg = 135,
        kLookAtNeck_OnGain = 136,
        kLookAtNeck_OffGain = 137,
        kLookAtNeck_OnLeadIn = 138,
        kLookAtNeck_OffLeadIn = 139,
        kLookAtNeck_Enabled = 140,
        kLookAtHead_FwdAxisLS = 141,
        kLookAtHead_LimitAngleDeg = 142,
        kLookAtHead_OnGain = 143,
        kLookAtHead_OffGain = 144,
        kLookAtHead_OnLeadIn = 145,
        kLookAtHead_OffLeadIn = 146,
        kLookAtHead_Enabled = 147,
        kLookAtLimitAngleDeg = 148,
        kLookAtLimitAngleThresholdDeg = 149,
        kLookAtOnGain = 150,
        kLookAtOffGain = 151,
        kLookAtOnLeadIn = 152,
        kLookAtOffLeadIn = 153,
        kLookAtUseBoneGains = 154,
        kLookAtAdditive = 155,
        kLookAtOutOfRange = 156,
        kLookAtAdditiveClamp = 157,
        kLookAtUseIndividualLimits = 158,
        kLookAtHead_LimitAngleDegVert = 159,
        kLookAtNeck_LimitAngleDegVert = 160,
        kLookAtChest_LimitAngleDegVert = 161,
        kLookAtSpine2_LimitAngleDegVert = 162,
        kiState_PlayerDefault = 163,
        kLookAt_RotateBoneAboutAxis = 164,
        kiState_NPCSneakingScreenspace = 165,
        kiState_NPCScreenspace = 166,
        kiState_NPCMelee = 167,
        km_errorOutTranslation = 168,
        km_alignWithGroundRotation = 169,
        km_worldFromModelFeedbackGain = 170,
        kbGraphWantsFootIK = 171,
        kbClampAdditive = 172,
        kfik_OnOffGain = 173,
        kfik_GroundAscendingGain = 174,
        kfik_GroundDescendingGain = 175,
        kfik_footplantedgain = 176,
        kfik_FootRaisedGain = 177,
        kfik_FootUnlockGain = 178,
        kfik_ErrorUpDownBias = 179,
        kLookAtHead_LimitAngleVert_Dwn = 180,
        kLookAtNeck_LimitAngleVert_Dwn = 181,
        kbHumanoidFootIKDisable = 182,
        kBoolVariable = 183,
        kIsPlayer = 184,
        kiState_NPCGun = 185,
        kbIsFemale = 186,
        kfRagdollAnimAmount = 187,
        kfHitReactionEndTimer = 188,
        kiState_PlayerMelee = 189,
        kiState_NPCFastWalk = 190,
        kbBlockPipboy = 191,
        kbFreezeSpeedUpdate = 192,
        kbDoNotInterrupt = 193,
        kbInLandingState = 194,
        kbEnableRoot_IsActiveMod = 195,
        kfRandomClipStartTimePercentage = 196,
        kWalkSpeedMult = 197,
        kJogSpeedMult = 198,
        krunSpeedMult = 199,
        kiSyncLocomotionSpeed = 200,
        kiControlsIdleSync = 201,
        kbPathingInterruptibleIdle = 202,
        kbBlockPOVSwitch = 203,
        kiIsPlayer = 204,
        kSpeedSmoothed = 205,
        ktestBlend = 206,
        kLeftHandIKOn = 207,
        kbIsThrowing = 208,
        kiSyncWalkRun = 209,
        kfIdleStopTime = 210,
        kiSyncJumpState = 211,
        kbTalkableWithItem = 212,
        kEnable_bEquipOK = 213,
        kiDynamicLoopStartState = 214,
        kfBodyMorphOffset = 215,
        kbIsInMT = 216,
        kbManualGraphChange = 217,
        kbFreezeRotationUpdate = 218,
        kiPcapTalkGenerator = 219,
        kDirectionDegrees = 220,
        kiWantBlock = 221,
        kiState_NPCBlocking = 222,
        kiSyncForwardBackward = 223,
        kIsBlocking = 224,
        kfDirectAtSavedGain = 225,
        kAimHeadingMaxCCW = 226,
        kAimHeadingMaxCW = 227,
        kiSyncForwardBackward00 = 228,
        kfPitchUpLimit = 229,
        kfPitchDownLimit = 230,
        kGunGripPointer = 231,
        kbDisableIsAttackReady = 232,
        kfLocomotionWalkPlaybackSpeed = 233,
        kfLocomotionJogPlaybackSpeed = 234,
        kfLocomotionRunPlaybackSpeed = 235,
        kAimPitchMaxUp = 236,
        kAimPitchMaxDown = 237,
        kiLocomotionSpeedState = 238,
        kbUseRifleReadyDirectAt = 239,
        kiMeleeState = 240,
        kTEMPIsPlayer = 241,
        kbEquipOkIsActiveEnabled = 242,
        kiSyncSneakWalkRun = 243,
        kiSyncDirection = 244,
        kfLocomotionSneakWalkPlaybackSpeed = 245,
        kfLocomotionSneakRunPlaybackSpeed = 246,
        kRightArmInjuredPowerFist = 247,
        kweaponSpeedMult = 248,
        kbIsSneaking = 249,
        kfVaultDistance = 250,
        kbDisableAttackReady = 251,
        kbSyncDirection = 252,
        kbActorMobilityNotFullyCrippled = 253,
        kbRootRifleEquipOk = 254,
        kbAnimateWeaponBones = 255,
        kfVaultHeight = 256,
        kbForceIdleStop = 257,
        kiSyncShuffleState = 258,
        kisSightedOver = 259,
        kbPartialCover = 260,
        kRightHandIKOn = 261,
        kiSyncSightedState = 262,
        kiWeaponChargeMode = 263,
        kLeftHandIKControlsModifierActive = 264,
        kReloadSpeedMult = 265,
        kiSyncReadyAlertRelaxed = 266,
        k_TestInt = 267,
        kfLeftHandIKFadeOut = 268,
        kfLeftHandIKTransformOnFraction = 269,
        kbUseLeftHandIKDefaults = 270,
        kiSyncGunDown = 271,
        kiRifleDrawnStateID = 272,
        kbAimEnabled = 273,
        kbShuffleSighted2 = 274,
        kbShuffleSighted = 275,
        kbAimCaptureEnabled = 276,
        kInt32Variable00 = 277,
        k_test = 278,
        k_Test2 = 279,
        kGunGripPointer_Mirrored = 280,
        kHandIKControlsDataActive = 281,
        kHandIKControlsDataActive_Mirrored = 282,
        kRifleDrawnCurrentState = 283,
        k_TestBool = 284,
        kCurrentJumpState = 285,
        kbRenderFirstPersonInWorld = 286,
        kbDisableSpineTracking = 287,
        kbAdjust1stPersonFOV = 288,
        kpipboyUp = 289,
        kfControllerXSum = 290,
        kfControllerYSum = 291,
        kfPACameraAdd = 292,
        kfTestVar = 293,
        kfBodyCameraRotation = 294,
        kfPlaybackMult = 295,
        kbIsInFlavor = 296,
        kiTalkGenerator = 297,
        kstaggerDirection = 298,
    };
}

std::optional<uint32_t> AnimationGraphDescriptor_Master_Behavior::TranslateThirdToFirstPerson(uint32_t aThirdPersonVar)
{
    switch (aThirdPersonVar)
    {
        // bools
    case TP::km_bEnablePitchTwistModifier:
        return FP::km_bEnablePitchTwistModifier;
    case TP::kIsSprinting:
        return FP::kIsSprinting;
    case TP::kisFiring:
        return FP::kisFiring;
    case TP::kisReloading:
        return FP::kisReloading;
    case TP::kiIsInSneak:
        return FP::kiIsInSneak;
    case TP::kisJumping:
        return FP::kisJumping;
    case TP::kbEquipOk:
        return FP::kbEquipOk;
    case TP::kIsStaggering:
        return FP::kisStagger;
    case TP::kbGraphDriven:
        return FP::kbGraphDriven;
    case TP::kbGraphDrivenRotation:
        return FP::kbGraphDrivenRotation;
    case TP::kbGraphWantsFootIK:
        return FP::kbGraphWantsFootIK;
    case TP::kbIsThrowing:
        return FP::kbIsThrowing;
    case TP::kIsBlocking:
        return FP::kIsBlocking;
    case TP::kbForceIdleStop:
        return FP::kbForceIdleStop;
    case TP::kbAllowHeadTracking:
        return FP::kbAllowHeadTracking;
    case TP::kbIsInFlavor:
        return FP::kbIsInFlavor;
    case TP::kbAimActive:
        return FP::kbAimActive;
    case TP::kbAllowRotation:
        return FP::kbAllowRotation;
    case TP::kbEnableRoot_IsActiveMod:
        return FP::kbEnableRoot_IsActiveMod;
    case TP::kbDisableSpineTracking:
        return FP::kbDisableSpineTracking;
    case TP::kIsNPC:
        return FP::kIsNPC;
    case TP::kIsPlayer:
        return FP::kIsPlayer;
    case TP::kbFreezeSpeedUpdate:
        return FP::kbFreezeSpeedUpdate;
    case TP::kbFreezeRotationUpdate:
        return FP::kbFreezeRotationUpdate;

        // floats
    case TP::kDirection:
        return FP::kDirection;
    case TP::kfSpeedRun:
        return FP::kfSpeedRun;
    case TP::kfSpeedWalk:
        return FP::kfSpeedWalk;
    case TP::kSpineTwist:
        return FP::kSpineTwist;
    case TP::kSpeed:
        return FP::kSpeed;
    case TP::kPitchOffset:
        return FP::kPitchOffset;
    case TP::kPitch:
        return FP::kPitch;
    case TP::kTurnDelta:
        return FP::kTurnDelta;
    case TP::kDirectionSmoothed:
        return FP::kDirectionSmoothed;
    case TP::kAimStability:
        return FP::kAimStability;
    case TP::kSpeedSmoothed:
        return FP::kSpeedSmoothed;
    case TP::kReloadSpeedMult:
        return FP::kReloadSpeedMult;
    case TP::kTurnDeltaSmoothed:
        return FP::kTurnDeltaSmoothed;
    case TP::kweaponSpeedMult:
        return FP::kweaponSpeedMult;
    case TP::kVelocityZ:
        return FP::kVelocityZ;
    case TP::kAimHeadingCurrent:
        return FP::kAimHeadingCurrent;
    case TP::kAimPitchCurrent:
        return FP::kAimPitchCurrent;
    case TP::kfDirectAtSavedGain:
        return FP::kfDirectAtSavedGain;

        // ints
    case TP::kiState:
        return FP::kiState;
    case TP::kiSyncSprintState:
        return FP::kiSyncSprintState;
    case TP::kiWeaponChargeMode:
        return FP::kiWeaponChargeMode;
    case TP::kiAttackState:
        return FP::kiAttackState;
    case TP::kiState_NPCSneaking:
        return FP::kiState_NPCSneaking;
    case TP::kiState_PlayerDefault:
        return FP::kiState_PlayerDefault;
    case TP::kiState_NPCGun:
        return FP::kiState_NPCGun;
    case TP::kiState_PlayerMelee:
        return FP::kiState_PlayerMelee;
    case TP::kiSyncIdleLocomotion:
        return FP::kiSyncIdleLocomotion;
    case TP::kiSyncJumpState:
        return FP::kiSyncJumpState;

    default:
        return std::nullopt;
    }
}

AnimationGraphDescriptor_Master_Behavior::AnimationGraphDescriptor_Master_Behavior(AnimationGraphDescriptorManager& aManager)
{
    // third person key
    uint64_t key = 8074503569708505439;

    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
        {TP::km_bEnablePitchTwistModifier,
            TP::kIsSprinting,
            TP::kisFiring,
            TP::kisReloading,
            TP::kIsAttackReady,
            TP::kisAttackNotReady,
            TP::kiIsInSneak,
            TP::kisJumping,
            TP::kbEquipOk,
            TP::kbInJumpState,
            TP::kIsStaggering,
            TP::kIsSneaking,
            TP::kisMirrored,
            TP::kbNotHeadTrack,
            TP::kbCCSupport,
            TP::kbCCOnStairs,
            TP::kbGraphDriven,
            TP::kbGraphDrivenTranslation,
            TP::kbGraphDrivenRotation,
            TP::kbGraphWantsFootIK,
            TP::kbIsFemale,
            TP::kbIsThrowing,
            TP::kEnable_bEquipOK,
            TP::kIsBlocking,
            TP::kbUseRifleReadyDirectAt,
            TP::kbEquipOkIsActiveEnabled,
            TP::kbIsSneaking,
            TP::kbAimEnabled,
            TP::kbForceIdleStop,
            TP::kbActorMobilityNotFullyCrippled,
            TP::kbSyncDirection,
            TP::kbDisableAttackReady,
            TP::kbAllowHeadTracking,
            TP::kbInLandingState,
            TP::kbIsInFlavor,
            TP::kbAimActive,
            TP::kbAllowRotation,
            TP::kbUseLeftHandIKDefaults,
            TP::kLeftHandIKOn,
            TP::kbEnableRoot_IsActiveMod,
            TP::kbIsInMT,
            TP::kbRootRifleEquipOk,
            TP::kbAimCaptureEnabled,
            TP::kbDisableSpineTracking,
            TP::kIsNPC,
            TP::kIsPlayer,
            TP::kbFreezeSpeedUpdate,
            TP::kbFreezeRotationUpdate,
        },
        {TP::kDirection,
            TP::kfSpeedRun,
            TP::kfSpeedWalk,
            TP::kSpineTwist,
            TP::kSpeed,
            TP::kPitchOffset,
            TP::kPitch,
            TP::kTurnDelta,
            TP::kDirectionSmoothed,
            TP::kAimStability,
            TP::kSampledSpeed,
            TP::kSpeedSmoothed,
            TP::kReloadSpeedMult,
            TP::kTurnDeltaSmoothed,
            TP::kWalkSpeedMult,
            TP::krunSpeedMult,
            TP::kDirectionDegrees,
            TP::kJogSpeedMult,
            TP::kweaponSpeedMult,
            TP::kfLocomotionWalkPlaybackSpeed,
            TP::kfLocomotionJogPlaybackSpeed,
            TP::kfLocomotionRunPlaybackSpeed,
            TP::kfLocomotionSneakRunPlaybackSpeed,
            TP::kfLocomotionSneakWalkPlaybackSpeed,
            TP::kfik_footplantedgain,
            TP::kVelocityZ,
            TP::kAimHeadingCurrent,
            TP::kAimPitchCurrent,
            TP::kfDirectAtSavedGain,
            TP::kfPlaybackMult,
            TP::kbAnimateWeaponBones,
        },
        {TP::kiState,
            TP::kiSyncSprintState,
            TP::kiWeaponChargeMode,
            TP::kiAttackState,
            TP::kiGetUpType,
            TP::kiState_Raider_Stumble_Rifle,
            TP::kiState_NPCSneaking,
            TP::kiState_PlayerDefault,
            TP::kiState_NPCMelee,
            TP::kiState_NPCGun,
            TP::kiState_PlayerMelee,
            TP::kiState_NPCFastWalk,
            TP::kiControlsIdleSync,
            TP::kiSyncWalkRun,
            TP::kiState_NPCBlocking,
            TP::kiLocomotionSpeedState,
            TP::kiMeleeState,
            TP::kCurrentJumpState,
            TP::kiSyncTurnState,
            TP::kbPathingInterruptibleIdle,
            TP::kiSyncLocomotionSpeed,
            TP::kiSyncShuffleState,
            TP::kiSyncSneakWalkRun,
            TP::kiSyncDirection,
            TP::kiSyncForwardBackward00,
            TP::kiSyncForwardBackward,
            TP::kiSyncIdleLocomotion,
            TP::kiSyncJumpState,
            TP::kiSyncReadyAlertRelaxed,
            TP::kiIsPlayer,
        }
    ));
}
