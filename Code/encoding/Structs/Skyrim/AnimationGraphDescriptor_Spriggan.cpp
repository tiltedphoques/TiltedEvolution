#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Spriggan.h>

AnimationGraphDescriptor_Spriggan::AnimationGraphDescriptor_Spriggan(AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kSpeed = 0,
        kfMinSpeed = 1,
        kbHeadTrackingOn = 2,
        kTargetLocation = 3,
        kcamerafromx = 4,
        kcamerafromy = 5,
        kcamerafromz = 6,
        kTurnDelta = 7,
        kiSyncTurnState = 8,
        kDirection = 9,
        kiState = 10,
        kiState_SprigganDefault = 11,
        kbHeadTracking = 12,
        kbCanHeadTrack = 13,
        kIsStaggering = 14,
        kIsRecoiling = 15,
        kIsAttacking = 16,
        kisCasting = 17,
        kIsSummoned = 18,
        kbWantCastLeft = 19,
        kbMLh_Ready = 20,
        kbWantCastRight = 21,
        kbMRh_Ready = 22,
        kblendDefault = 23,
        kblendSlow = 24,
        kIntDirection = 25,
        kIsAttackReady = 26,
        kiSyncIdleLocomotion = 27,
        kbAnimationDriven = 28,
        kiSyncDefaultState = 29,
        kblendFast = 30,
        kstaggerMagnitude = 31,
        kSpeedSampled = 32,
        kSpeedSampledGain = 33,
        kSpeedSampledDamped = 34,
        kDirectionGain = 35,
        kbEquipOk = 36,
        kIsDualActive = 37,
        kiState_SprigganCombat = 38,
        kbNoHeadTrack = 39,
        kiGetUpType = 40,
        kbIsSynced = 41,
        kbForceIdleStop = 42,
        kstaggerDirection = 43,
    };

    uint64_t key = 10099378323021197839;

    AnimationGraphDescriptorManager::Builder s_builder(
        aManager, key,
        AnimationGraphDescriptor(
            {
                kbEquipOk,
                kbAnimationDriven,
                kIsAttackReady,
                kIsRecoiling,
                kIsStaggering,
                kIsAttacking,
                kbHeadTracking,
                kbHeadTrackingOn,
                kbCanHeadTrack,
                kbMLh_Ready,
                kbMRh_Ready,
                kbNoHeadTrack,
                kisCasting,
                kbWantCastLeft,
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
                kiState,
            }));
}
