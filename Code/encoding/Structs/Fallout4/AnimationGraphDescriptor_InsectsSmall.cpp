#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_InsectsSmall.h>



AnimationGraphDescriptor_InsectsSmall::AnimationGraphDescriptor_InsectsSmall(
    AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kiState = 0,
        kiState_RadRoach = 1,
        kbGraphDriven = 2,
        kbGraphDrivenRotation = 3,
        kbGraphDrivenTranslation = 4,
        kbAnimationDriven = 5,
        kbAllowRotation = 6,
        kstaggerDirection = 7,
        kstaggerMagnitude = 8,
        kiCombatLocomotionState = 9,
        kiCombatStandingState = 10,
        kiNonCombatLocomotionState = 11,
        kiNonCombatStandingState = 12,
        kiGetUpType = 13,
        kiCombatState = 14,
        kiSyncTurnState = 15,
        kiSyncIdleLocomotion = 16,
        kSpeed = 17,
        kIsAttackReady = 18,
        kbEnableAttackMod = 19,
        kDirection = 20,
        kbEquipOk = 21,
        kfRandomClipStartTimePercentage = 22,
        kidleTimeMax = 23,
        kidleTimeMin = 24,
        kiDynamicAnimSelector = 25,
        kbAllowHeadTracking = 26,
        kLookAtOutOfRange = 27,
        kcamerafromx = 28,
        kcamerafromy = 29,
        kcamerafromz = 30,
        kLookAtChest_Enabled = 31,
        kLookAtSpine2_Enabled = 32,
        kbFreezeSpeedUpdate = 33,
        kiIsInSneak = 34,
        kbGraphWantsFootIK = 35,
        kbRenderFirstPersonInWorld = 36,
        kbDisableSpineTracking = 37,
        kIsPlayer = 38,
        kbBlockPOVSwitch = 39,
        kbBlockPipboy = 40,
        kPose = 41,
        kbAdjust1stPersonFOV = 42,
        kiRecoilSelector = 43,
        kTurnDelta = 44,
        kIsBlocking = 45,
        kiSyncWalkRun = 46,
        kbManualGraphChange = 47,
        krecoilShortMult = 48,
        kLookAtOutsideLimit = 49,
        kbForceIdleStop = 50,
        kbDoNotInterrupt = 51,
        kLookAtLimitAngleDeg = 52,
        kLookAtChest_LimitAngleDeg = 53,
        kLookAtNeck_LimitAngleDeg = 54,
        kLookAtHead_LimitAngleDeg = 55,
        kLookAtHead_OnGain = 56,
        kLookAtAdditive = 57,
        kbEnableRoot_IsActiveMod = 58,
        kIsNPC = 59,
        kLookAtOnGain = 60,
        kLookAtOffGain = 61,
        kbTalkableWithItem = 62,
        kiTalkGenerator = 63,
        kbFreezeRotationUpdate = 64,
        kiPcapTalkGenerator = 65,
        kSpeedSmoothed = 66,
        kTurnDeltaSmoothed = 67,
        kbIsInFlavor = 68,
    };

    uint64_t key = 11398773395717218432;

    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
            {
                kbAnimationDriven,
                kIsAttackReady,
                kbEquipOk,
            },
            {
                kDirection,
                kstaggerDirection,
                kTurnDeltaSmoothed,
                kSpeedSmoothed,
                kTurnDelta,
                kSpeed,
            },
            {
                kiSyncIdleLocomotion,
                kiSyncTurnState,
                kiCombatState,
                kiRecoilSelector,
            }
        ));
}
