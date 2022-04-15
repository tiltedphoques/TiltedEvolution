#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_TrollBehavior.h>


AnimationGraphDescriptor_TrollBehavior::AnimationGraphDescriptor_TrollBehavior(AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kiSyncTurnState = 0,
        kTurnDelta = 1,
        kDirection = 2,
        kSpeed = 3,
        kbAnimationDriven = 4,
        kbIsSynced = 5,
        kIsRecoiling = 6,
        kIsStaggering = 7,
        kSpeedStartVar = 8,
        kTurnDeltaDamped = 9,
        kTurnMin = 10,
        kIsAttackReady = 11,
        kIntDirection = 12,
        kSpeedDamped = 13,
        kIsAttacking = 14,
        kbAllowRotation = 15,
        kFootIKEnable = 16,
        kbHeadTrackingOff = 17,
        kstaggerMagnitude = 18,
        kbEquipOk = 19,
        kcurrentDefaultState = 20,
        kiState = 21,
        kiState_TrollDefault = 22,
        kSampledSpeed = 23,
        kIsCrouching = 24,
        kiSyncIdleLocomotion = 25,
        kblendShort = 26,
        kblendNormal = 27,
        kisMoving = 28,
        kDirectionOffset = 29,
        kDirDamped = 30,
        kTimeDelta = 31,
        kDirAcc = 32,
        kDirectionBlendA = 33,
        kDirectionBlendB = 34,
        kActiveBlend = 35,
        kbDirBlendA = 36,
        kbDirBlendB = 37,
        kblendThresh = 38,
        kblendAttack = 39,
        kbHeadTracking = 40,
        kTargetLocation = 41,
        kcamerafromx = 42,
        kcamerafromy = 43,
        kcamerafromz = 44,
        kiGetUpType = 45,
        kstaggerDirection = 46,
        kbForceIdleStop = 47,
    };

    uint64_t key = 12972242470338891659;
    
    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
        {kbAnimationDriven,
            kIsAttackReady,
            kbAllowRotation,
            kbHeadTrackingOff,
            kisMoving,
            kIsAttacking},
        {kTurnDelta,
            kDirection,
            kSpeed,
            kSampledSpeed,
            kDirDamped,
            kDirectionBlendA},
        {kiSyncTurnState,
            kiSyncIdleLocomotion}));
}
