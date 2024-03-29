#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Ghoul.h>

AnimationGraphDescriptor_Ghoul::AnimationGraphDescriptor_Ghoul(AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kcHitReactionBodyPart = 0,
        kiState = 1,
        kiState_FeralGhoul = 2,
        kiState_FeralGhoulSprint = 3,
        kDirection = 4,
        kiGetUpType = 5,
        kbAnimationDriven = 6,
        kiCombatState = 7,
        kbHeadTrackingEnabled = 8,
        kfHeadTrackingGainOn = 9,
        kfHeadTrackingGainOff = 10,
        kcamerafromx = 11,
        kcamerafromy = 12,
        kcamerafromz = 13,
        kLookAtOutOfRange = 14,
        kbAllowHeadTracking = 15,
        kbHeadTrackingDebug = 16,
        kIsAttackReady = 17,
        kbGraphWantsFootIK = 18,
        km_alignWithGroundRotation = 19,
        km_errorOutTranslation = 20,
        km_worldFromModelFeedbackGain = 21,
        kfTimeStep = 22,
        kfSpineTwistGainAdj = 23,
        kfSpineTwistGain = 24,
        kSpineXTwist = 25,
        kSpineYTwist = 26,
        kSpineZTwist = 27,
        kfHeadTwistGainAdj = 28,
        kfHeadTwistGain = 29,
        kHeadXTwist = 30,
        kHeadYTwist = 31,
        kHeadZTwist = 32,
        kHeadTwistMinMax = 33,
        kSpineTwistMinMax = 34,
        kfHitReactionEndTimer = 35,
        kbReactsEnabled = 36,
        kbSpineEnabled = 37,
        kbHumanoidFootIKDisable = 38,
        kfik_footplantedgain = 39,
        kfRandomClipStartTimePercentage = 40,
        kiSyncTurnState = 41,
        kSpeed = 42,
        kiSyncIdleLocomotion = 43,
        kbEnableAttackMod = 44,
        kbEquipOk = 45,
        kIsBlocking = 46,
        kbFreezeSpeedUpdate = 47,
        kiSyncSprintState = 48,
        kidleTimeMax = 49,
        kidleTimeMin = 50,
        kIsSprinting = 51,
        kIsCastingLeft = 52,
        kIsCastingRight = 53,
        kbRitualSpellActive = 54,
        kbMRh_Ready = 55,
        kbMLh_Ready = 56,
        kbWantCastLeft = 57,
        kbWantCastRight = 58,
        kbAttached = 59,
        kCastBlend = 60,
        kCastBlendDamped = 61,
        kCastBlendGained = 62,
        kNotCasting = 63,
        kIsNPC = 64,
        kAimGainOn = 65,
        kAimGainOff = 66,
        kAimPitchCurrent = 67,
        kAimHeadingCurrent = 68,
        kbAimActive = 69,
        kbIsSynced = 70,
        kbGraphDrivenRotation = 71,
        kTurnDelta = 72,
        kbManualGraphChange = 73,
        kRightArmInjured = 74,
        kLeftArmInjured = 75,
        kiInjuredArmState = 76,
        kTurnDeltaSmoothed = 77,
        kbInJumpState = 78,
        kbGraphDriven = 79,
        kbBlockMoveStop = 80,
        kbInCombat = 81,
        kisAttacking = 82,
        kiSyncFootState = 83,
        kbAllowWalkEvents = 84,
        kSpeedSmoothed = 85,
        kbAllowRotation = 86,
        kbFreezeDirectionUpdate = 87,
        kbRunning = 88,
        kiDynamicAnimSelector = 89,
        kfWalkPlaybackSpeedMult = 90,
        kfRunSpeedPlaybackMult = 91,
        kLookAtChest_Enabled = 92,
        kLookAtSpine2_Enabled = 93,
        kiIsInSneak = 94,
        kbRenderFirstPersonInWorld = 95,
        kPose = 96,
        kbDisableSpineTracking = 97,
        kIsPlayer = 98,
        kbBlockPOVSwitch = 99,
        kbBlockPipboy = 100,
        kbAdjust1stPersonFOV = 101,
        kiRecoilSelector = 102,
        kstaggerDirection = 103,
        kstaggerMagnitude = 104,
        kiNonCombatStandingState = 105,
        kiNonCombatLocomotionState = 106,
        kiCombatStandingState = 107,
        kiCombatLocomotionState = 108,
        kiSyncWalkRun = 109,
        krecoilShortMult = 110,
        kLookAtOutsideLimit = 111,
        kbForceIdleStop = 112,
        kbDoNotInterrupt = 113,
        kLookAtLimitAngleDeg = 114,
        kLookAtChest_LimitAngleDeg = 115,
        kLookAtNeck_LimitAngleDeg = 116,
        kLookAtHead_LimitAngleDeg = 117,
        kLookAtHead_OnGain = 118,
        kLookAtAdditive = 119,
        kbEnableRoot_IsActiveMod = 120,
        kLookAtOnGain = 121,
        kLookAtOffGain = 122,
        kbTalkableWithItem = 123,
        kiTalkGenerator = 124,
        kbFreezeRotationUpdate = 125,
        kiPcapTalkGenerator = 126,
        kbIsInFlavor = 127,
    };
    uint64_t key = 18279284073093955153;

    AnimationGraphDescriptorManager::Builder s_builder(
        aManager, key,
        AnimationGraphDescriptor(
            {
                kbEquipOk,
                kbAllowHeadTracking,
                kbAnimationDriven,
                kLookAtOutOfRange,
                kbManualGraphChange,
                kIsAttackReady,
                kIsSprinting,
            },
            {
                kfHeadTwistGainAdj,
                kfSpineTwistGainAdj,
                kstaggerMagnitude,
                kfRunSpeedPlaybackMult,
                kSpineZTwist,
                kfik_footplantedgain,
                kstaggerDirection,
                kfTimeStep,
                kfHitReactionEndTimer,
                kTurnDeltaSmoothed,
                kfWalkPlaybackSpeedMult,
                kSpeedSmoothed,
                kDirection,
                kSpineYTwist,
                kSpineXTwist,
                kTurnDelta,
                kSpeed,
            },
            {
                kiCombatState,
                kiSyncTurnState,
                kiState,
                kiSyncSprintState,
                kiSyncIdleLocomotion,
                kiRecoilSelector,
                kcHitReactionBodyPart,
            }));
}
