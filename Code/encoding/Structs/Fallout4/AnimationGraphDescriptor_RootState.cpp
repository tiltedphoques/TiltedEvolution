#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_RootState.h>


AnimationGraphDescriptor_RootState::AnimationGraphDescriptor_RootState(
    AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kiState = 0,
        kiState_Cat = 1,
        kbGraphDriven = 2,
        kbAnimationDriven = 3,
        kbSupportedDeathAnim = 4,
        kiGetUpType = 5,
        kSpeed = 6,
        kDirection = 7,
        kTurnDelta = 8,
        kSpeedSmoothed = 9,
        kfRunPlaybackSpeedMult = 10,
        kTurnDeltaSmoothed = 11,
        kfWalkPlaybackSpeedMult = 12,
        kiSyncLocomotionState = 13,
        kbForceIdleStop = 14,
        kiSyncIdleLocomotion = 15,
        kiSyncTurnState = 16,
        kbAllowRotation = 17,
        kbAllowHeadTracking = 18,
        kLookAtOutOfRange = 19,
        kcamerafromx = 20,
        kcamerafromy = 21,
        kcamerafromz = 22,
        kfRandomClipStartTimePercentage = 23,
        kLookAtChest_Enabled = 24,
        kLookAtSpine2_Enabled = 25,
        kbFreezeSpeedUpdate = 26,
        kiIsInSneak = 27,
        kbGraphWantsFootIK = 28,
        kbRenderFirstPersonInWorld = 29,
        kbDisableSpineTracking = 30,
        kIsPlayer = 31,
        kbBlockPOVSwitch = 32,
        kbBlockPipboy = 33,
        kPose = 34,
        kbAdjust1stPersonFOV = 35,
        kLookAtOutsideLimit = 36,
        kbDoNotInterrupt = 37,
        kLookAtLimitAngleDeg = 38,
        kLookAtChest_LimitAngleDeg = 39,
        kLookAtNeck_LimitAngleDeg = 40,
        kLookAtHead_LimitAngleDeg = 41,
        kLookAtHead_OnGain = 42,
        kLookAtAdditive = 43,
        kbEnableRoot_IsActiveMod = 44,
        kIsNPC = 45,
        kLookAtOnGain = 46,
        kLookAtOffGain = 47,
        kbTalkableWithItem = 48,
        kiTalkGenerator = 49,
        kbFreezeRotationUpdate = 50,
        kbManualGraphChange = 51,
        kiPcapTalkGenerator = 52,
        kbIsInFlavor = 53,
    };

    uint64_t key = 16544277667400076734;

    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
            {
                kbAnimationDriven,
                kbForceIdleStop,
            },
            {
                kDirection,
                kSpeed,
                kfWalkPlaybackSpeedMult,
                kSpeedSmoothed,
                kTurnDelta,
                kTurnDeltaSmoothed,
                kfRunPlaybackSpeedMult,
            },
            {
                kiSyncTurnState,
                kiSyncLocomotionState,
                kiSyncIdleLocomotion,
            }));
}
