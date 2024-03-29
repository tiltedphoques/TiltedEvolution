#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_VampireBruteRootBehavior.h>

AnimationGraphDescriptor_VampireBruteRootBehavior::AnimationGraphDescriptor_VampireBruteRootBehavior(AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kiSyncIdleLocomotion = 0,
        kSpeed = 1,
        kDirection = 2,
        kbAnimationDriven = 3,
        kBlendNormal = 4,
        kIsAttacking = 5,
        kblendFast = 6,
        kTurnDelta = 7,
        kiSyncTurnState = 8,
        kIsAttackReady = 9,
        kIsRecoiling = 10,
        kIsStaggering = 11,
        kstaggerMagnitude = 12,
        kTargetLocation = 13,
        kIsBleedingOut = 14,
        kblendSlow = 15,
        kFootIKEnable = 16,
        km_onOffGain = 17,
        km_groundAscendingGain = 18,
        km_groundDescendingGain = 19,
        km_footPlantedGain = 20,
        km_footRaisedGain = 21,
        km_footUnlockGain = 22,
        km_worldFromModelFeedbackGain = 23,
        km_errorUpDownBias = 24,
        km_alignWorldFromModelGain = 25,
        km_hipOrientationGain = 26,
        km_footPlantedAnkleHeightMS = 27,
        km_footRaisedAnkleHeightMS = 28,
        km_maxAnkleHeightMS = 29,
        km_minAnkleHeightMS = 30,
        kbEquipOk = 31,
        kiSyncDefaultState = 32,
        kbHeadTrackingOn = 33,
        kbCanHeadTrack = 34,
        kfMinSpeed = 35,
        kfMinTurnSpeed = 36,
        kiState = 37,
        kiState_GargoyleDefault = 38,
        km_footEndLS = 39,
        kSpeedSampled = 40,
        kIsBashing = 41,
        kiState_GargoyleCombatRun = 42,
        kcamerafromx = 43,
        kcamerafromy = 44,
        kcamerafromz = 45,
        kDirectionDamped = 46,
        kDirectionGain = 47,
        km_raycastDistanceUp = 48,
        km_raycastDistanceDown = 49,
        kstaggerDirection = 50,
        km_errorOutTranslation = 51,
        kbAllowRotation = 52,
        kiGetUpType = 53,
        kbHeadTracking = 54,
        kbIsSynced = 55,
        kiStateRunWalk = 56,
        kiState_GargoyleCombatWalk = 57,
        kiStateCurrent = 58,
        kIsBleedingOutTransition = 59,
    };

    uint64_t key = 6408297713843182476;

    AnimationGraphDescriptorManager::Builder s_builder(
        aManager, key,
        AnimationGraphDescriptor(
            {
                kbEquipOk,
                kbAnimationDriven,
                kIsAttackReady,
                kbAllowRotation,
                kIsRecoiling,
                kIsStaggering,
                kIsAttacking,
                kbHeadTracking,
                kbHeadTrackingOn,
                kbCanHeadTrack,
            },
            {
                kSpeed,
                kTurnDelta,
                kDirection,
                kSpeedSampled,
            },
            {
                kiSyncDefaultState,
                kiSyncIdleLocomotion,
                kiSyncTurnState,
            }));
}
