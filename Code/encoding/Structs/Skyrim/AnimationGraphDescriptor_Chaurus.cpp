#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Chaurus.h>

AnimationGraphDescriptor_Chaurus::AnimationGraphDescriptor_Chaurus(AnimationGraphDescriptorManager& aManager)
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
        km_onOffGain = 11,
        km_groundAscendingGain = 12,
        km_groundDescendingGain = 13,
        km_footPlantedGain = 14,
        km_footRaisedGain = 15,
        km_footUnlockGain = 16,
        km_worldFromModelFeedbackGain = 17,
        km_errorUpDownBias = 18,
        km_alignWorldFromModelGain = 19,
        km_hipOrientationGain = 20,
        km_footPlantedAnkleHeightMS = 21,
        km_footRaisedAnkleHeightMS = 22,
        km_maxAnkleHeightMS = 23,
        km_minAnkleHeightMS = 24,
        kbAllowRotation = 25,
        kturnSpeedMult = 26,
        kiCombatStance = 27,
        kiSyncTurnState = 28,
        kIntDirection = 29,
        kbMotionDriven = 30,
        kiSyncIdleLocomotion = 31,
        kfMinTurnDelta = 32,
        kbAnimationDriven = 33,
        kBoolVariable00 = 34,
        kIsAttacking = 35,
        kiLeftHandType = 36,
        kiWeapCategory = 37,
        kbWantCastLeft = 38,
        kbMLh_Ready = 39,
        kiState_DefaultChaurus_MT = 40,
        kiState = 41,
        kIsBashing = 42,
        kbEquipOk = 43,
        kIsStaggering = 44,
        kIsRecoiling = 45,
        km_state = 46,
        km_direction = 47,
        km_goalSpeed = 48,
        km_speedOut = 49,
        kSpeedDamped = 50,
        km_enable = 51,
        km_limitAngleDegrees = 52,
        km_startBoneIndex = 53,
        km_gain = 54,
        km_boneRadius = 55,
        km_endBoneIndex = 56,
        km_castOffset = 57,
        kSpeedSampled = 58,
        kbHeadTracking = 59,
        kTargetLocation = 60,
        kbHeadTrackingOff = 61,
        kSlowBlend = 62,
        kbNoHeadTrack = 63,
        kAggroWarningBlend = 64,
        kstaggerDirection = 65,
        kiCurrentStateID = 66,
        kiGetUpType = 67,
        kIsSideAttacking = 68,
        kisIdleLay = 69,
        kisIdleSitting = 70,
        kspeedRight = 71,
        kspeedMultRight = 72,
        kspeedLeft = 73,
        kspeedMultLeft = 74,
        kspeedBackward = 75,
        kspeedMultBackward = 76,
        kspeedForward = 77,
        kspeedMultForward = 78,
    };

    uint64_t key = 6432093022549018934;

    AnimationGraphDescriptorManager::Builder s_builder(
        aManager, key, AnimationGraphDescriptor({kIsAttackReady, kbAllowRotation, kbAnimationDriven, kbMLh_Ready, kbEquipOk, kIsRecoiling}, {kDirection, kSpeed, kTurnDelta, kturnSpeedMult, kSpeedSampled, kspeedMultRight, kspeedMultLeft, kspeedMultForward}, {kiSyncTurnState, kiSyncIdleLocomotion}));
}
