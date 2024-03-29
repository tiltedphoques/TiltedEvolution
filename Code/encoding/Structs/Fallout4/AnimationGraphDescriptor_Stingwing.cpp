#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Stingwing.h>

AnimationGraphDescriptor_Stingwing::AnimationGraphDescriptor_Stingwing(AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kiState = 0,
        kiState_Stingwing = 1,
        kbGraphDriven = 2,
        kbAnimationDriven = 3,
        kbAllowRotation = 4,
        kcHitReactionBodyPart = 5,
        kiGetUpType = 6,
        kiCombatState = 7,
        kbManualGraphChange = 8,
        kfTimeStep = 9,
        kfSpineTwistGainAdj = 10,
        kfSpineTwistGain = 11,
        kSpineXTwist = 12,
        kSpineYTwist = 13,
        kSpineZTwist = 14,
        kfHitReactionEndTimer = 15,
        kiSyncTurnState = 16,
        kiNonCombatStandingState = 17,
        kiNonCombatLocomotionState = 18,
        kiSyncIdleLocomotion = 19,
        kSpeed = 20,
        kiCombatStandingState = 21,
        kiCombatLocomotionState = 22,
        kIsAttackReady = 23,
        kbEnableAttackMod = 24,
        kDirection = 25,
        kbEquipOk = 26,
        kfRandomClipStartTimePercentage = 27,
        kTurnDeltaSmoothed = 28,
        kidleTimeMax = 29,
        kidleTimeMin = 30,
        kiDynamicAnimSelector = 31,
        kiCombatState00 = 32,
        kbSupportedDeathAnim = 33,
        kbAllowHeadTracking = 34,
        kLookAtOutOfRange = 35,
        kcamerafromx = 36,
        kcamerafromy = 37,
        kcamerafromz = 38,
        kLookAtChest_Enabled = 39,
        kLookAtSpine2_Enabled = 40,
        kbFreezeSpeedUpdate = 41,
        kiIsInSneak = 42,
        kbGraphWantsFootIK = 43,
        kbRenderFirstPersonInWorld = 44,
        kbDisableSpineTracking = 45,
        kIsPlayer = 46,
        kbBlockPOVSwitch = 47,
        kbBlockPipboy = 48,
        kPose = 49,
        kbAdjust1stPersonFOV = 50,
        kiRecoilSelector = 51,
        kstaggerDirection = 52,
        kstaggerMagnitude = 53,
        kTurnDelta = 54,
        kIsBlocking = 55,
        kiSyncWalkRun = 56,
        krecoilShortMult = 57,
        kLookAtOutsideLimit = 58,
        kbForceIdleStop = 59,
        kbDoNotInterrupt = 60,
        kLookAtLimitAngleDeg = 61,
        kLookAtChest_LimitAngleDeg = 62,
        kLookAtNeck_LimitAngleDeg = 63,
        kLookAtHead_LimitAngleDeg = 64,
        kLookAtHead_OnGain = 65,
        kLookAtAdditive = 66,
        kbEnableRoot_IsActiveMod = 67,
        kIsNPC = 68,
        kLookAtOnGain = 69,
        kLookAtOffGain = 70,
        kbTalkableWithItem = 71,
        kiTalkGenerator = 72,
        kbFreezeRotationUpdate = 73,
        kiPcapTalkGenerator = 74,
        kSpeedSmoothed = 75,
        kbIsInFlavor = 76,
    };
    uint64_t key = 1567904913354835406;

    AnimationGraphDescriptorManager::Builder s_builder(
        aManager, key,
        AnimationGraphDescriptor(
            {
                kbSupportedDeathAnim,
                kbAnimationDriven,
                kIsAttackReady,
                kbEquipOk,
                kbManualGraphChange,
            },
            {
                kSpeed,
                kTurnDelta,
                kTurnDeltaSmoothed,
                kSpeedSmoothed,
                kstaggerMagnitude,
                kstaggerDirection,
                kDirection,
            },
            {
                kiCombatState,
                kiSyncTurnState,
                kiSyncIdleLocomotion,
                kiRecoilSelector,
            }));
}
