#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Rabbit.h>

AnimationGraphDescriptor_Rabbit::AnimationGraphDescriptor_Rabbit(AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kblendDefault = 0,
        kblendFast = 1,
        kblendSlow = 2,
        kDirection = 3,
        kIsBlocking = 4,
        kSpeed = 5,
        kstaggerMagnitude = 6,
        kTurnDelta = 7,
        kIsAttackReady = 8,
        kweaponSpeedMult = 9,
        kFootIKEnable = 10,
        kbAllowRotation = 11,
        kturnSpeedMult = 12,
        kiCombatStance = 13,
        kiSyncTurnDirection = 14,
        kIntDirection = 15,
        kbMotionDriven = 16,
        kiSyncIdleLocomotion = 17,
        kfMinTurnDelta = 18,
        kbAnimationDriven = 19,
        kBoolVariable00 = 20,
        kIsAttacking = 21,
        kiLeftHandType = 22,
        kiWeapCategory = 23,
        kbWantCastLeft = 24,
        kbMLh_Ready = 25,
        kiState_Hare_Default_MT = 26,
        kiState = 27,
        kIsBashing = 28,
        kIsStaggering = 29,
        kIsRecoiling = 30,
        kSpeedDamped = 31,
        kSpeedSampled = 32,
        kbHeadTracking = 33,
        kTargetLocation = 34,
        kbHeadTrackingOff = 35,
        kSlowBlend = 36,
        kbNoHeadTrack = 37,
        kAggroWarningBlend = 38,
        kstaggerDirection = 39,
        kiCurrentStateID = 40,
        kiGetUpType = 41,
        kEC_iSyncIdleLocomotion_1 = 42,
        kisIdleSitting = 43,
        kisIdleLay = 44,
        kbForceIdleStop = 45,
    };

    uint64_t key = 11071881714804970071;

    AnimationGraphDescriptorManager::Builder s_builder(
        aManager, key,
        AnimationGraphDescriptor(
            {
                kbAnimationDriven,
                kbAllowRotation,
                kbHeadTracking,
                kIsRecoiling,
                kIsStaggering,
                kIsAttacking,
            },
            {
                kSpeed,
                kTurnDelta,
                kturnSpeedMult,
                kDirection,
                kSpeedSampled,
            },
            {
                kiSyncIdleLocomotion,
                kiCombatStance,
            }));
}
