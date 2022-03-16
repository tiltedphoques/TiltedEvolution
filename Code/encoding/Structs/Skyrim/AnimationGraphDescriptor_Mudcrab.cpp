#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Mudcrab.h>



AnimationGraphDescriptor_Mudcrab::AnimationGraphDescriptor_Mudcrab(AnimationGraphDescriptorManager& aManager)
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
        kFootIKEnable = 9,
        kbAllowRotation = 10,
        kturnSpeedMult = 11,
        kiCombatStance = 12,
        kiSyncTurnState = 13,
        kIntDirection = 14,
        kbMotionDriven = 15,
        kiSyncIdleLocomotion = 16,
        kfMinTurnDelta = 17,
        kbAnimationDriven = 18,
        kBoolVariable00 = 19,
        kIsAttacking = 20,
        kiLeftHandType = 21,
        kiWeapCategory = 22,
        kbWantCastLeft = 23,
        kiState_MCrab_Default_MT = 24,
        kiState = 25,
        kIsBashing = 26,
        kbEquipOk = 27,
        kIsStaggering = 28,
        kIsRecoiling = 29,
        kSpeedDamped = 30,
        kSpeedSampled = 31,
        kbHeadTracking = 32,
        kTargetLocation = 33,
        kbHeadTrackingOff = 34,
        kSlowBlend = 35,
        kbNoHeadTrack = 36,
        kAggroWarningBlend = 37,
        kstaggerDirection = 38,
        kiCurrentStateID = 39,
        kisIdleLay = 40,
        kisIdleSitting = 41,
        kiGetUpType = 42,
        kIsSideAttacking = 43,
    };

    uint64_t key = 9498225481650921683;

    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
        {kbAnimationDriven,
            kbAllowRotation,
            kbHeadTracking,
            kIsRecoiling,
            kIsStaggering,
            kIsAttacking,
            kIsAttackReady,
            kbEquipOk,
            },
        {kSpeed,
            kTurnDelta,
            kturnSpeedMult,
            kDirection,
            kSpeedSampled,
            },
        {kiSyncIdleLocomotion,
            kiSyncTurnState,
            kiCombatStance,
            kiCurrentStateID,
            }));
}
