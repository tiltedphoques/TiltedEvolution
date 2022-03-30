#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Bloatfly.h>



AnimationGraphDescriptor_Bloatfly::AnimationGraphDescriptor_Bloatfly(
    AnimationGraphDescriptorManager& aManager)
{
    uint64_t key = 10733037448866675267;

    enum Variables
    {
        kiState = 0,
        kiState_Bloatfly = 1,
        kbGraphDriven = 2,
        kbAnimationDriven = 3,
        kbAllowRotation = 4,
        kiGetUpType = 5,
        kiCombatState = 6,
        kbHeadTrackingEnabled = 7,
        kbGraphWantsHeadTracking = 8,
        kLookAtOutOfRange = 9,
        kcamerafromx = 10,
        kcamerafromy = 11,
        kcamerafromz = 12,
        kbHeadTrackingDebug = 13,
        kiSyncTurnState = 14,
        kiNonCombatStandingState = 15,
        kiNonCombatLocomotionState = 16,
        kiSyncIdleLocomotion = 17,
        kSpeed = 18,
        kiCombatStandingState = 19,
        kiCombatLocomotionState = 20,
        kIsAttackReady = 21,
        kbEnableAttackMod = 22,
        kDirection = 23,
        kbEquipOk = 24,
        kfRandomClipStartTimePercentage = 25,
        kiDynamicAnimSelector = 26,
        kbAllowHeadTracking = 27,
        kLookAtChest_Enabled = 28,
        kLookAtSpine2_Enabled = 29,
        kbFreezeSpeedUpdate = 30,
        kiIsInSneak = 31,
        kbGraphWantsFootIK = 32,
        kbRenderFirstPersonInWorld = 33,
        kbDisableSpineTracking = 34,
        kIsPlayer = 35,
        kbBlockPOVSwitch = 36,
        kbBlockPipboy = 37,
        kPose = 38,
        kbAdjust1stPersonFOV = 39,
        kiRecoilSelector = 40,
        kstaggerDirection = 41,
        kstaggerMagnitude = 42,
        kTurnDelta = 43,
        kIsBlocking = 44,
        kiSyncWalkRun = 45,
        kbManualGraphChange = 46,
        krecoilShortMult = 47,
        kLookAtOutsideLimit = 48,
        kbForceIdleStop = 49,
        kbDoNotInterrupt = 50,
        kLookAtLimitAngleDeg = 51,
        kLookAtChest_LimitAngleDeg = 52,
        kLookAtNeck_LimitAngleDeg = 53,
        kLookAtHead_LimitAngleDeg = 54,
        kLookAtHead_OnGain = 55,
        kLookAtAdditive = 56,
        kbEnableRoot_IsActiveMod = 57,
        kIsNPC = 58,
        kLookAtOnGain = 59,
        kLookAtOffGain = 60,
        kbTalkableWithItem = 61,
        kiTalkGenerator = 62,
        kbFreezeRotationUpdate = 63,
        kiPcapTalkGenerator = 64,
        kSpeedSmoothed = 65,
        kTurnDeltaSmoothed = 66,
        kbIsInFlavor = 67,
    };

    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
            {
                kbAnimationDriven,
                kbEquipOk,
                kLookAtOutOfRange,
                kIsAttackReady,
                kbGraphWantsHeadTracking,
            },
            {
                kstaggerDirection,
                kDirection,
                kTurnDeltaSmoothed,
                kSpeedSmoothed,
                kSpeed,
                kTurnDelta,
                kstaggerMagnitude,
            },
            {
                kiCombatState,
                kiSyncTurnState,
                kiSyncIdleLocomotion,
            }
        ));
}
