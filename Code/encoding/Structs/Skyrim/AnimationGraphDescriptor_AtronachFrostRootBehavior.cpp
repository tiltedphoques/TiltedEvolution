#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_AtronachFrostRootBehavior.h>



AnimationGraphDescriptor_AtronachFrostRootBehavior::AnimationGraphDescriptor_AtronachFrostRootBehavior(AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kDefaultBlend = 0,
        kDirection = 1,
        kiSyncIdleLocomotion = 2,
        kSpeed = 3,
        kIsStaggering = 4,
        kbEquipOk = 5,
        kIsRecoiling = 6,
        kbAnimationDriven = 7,
        kIsAttackReady = 8,
        kIsAttacking = 9,
        kbAllowRotation = 10,
        kfMinSpeed = 11,
        kstaggerMagnitude = 12,
        kiSyncDefaultState = 13,
        kFastBlend = 14,
        kIsBlocking = 15,
        kIsBashing = 16,
        kiWantBlock = 17,
        kiState_AtronachFrostBlocking = 18,
        kiState = 19,
        kiState_AtronachFrostDefault = 20,
        kTurnDelta = 21,
        kiSyncTurnState = 22,
        kTurnDamped = 23,
        kSpeedAcc = 24,
        kTimeDelta = 25,
        kIsSummoned = 26,
        km_onOffGain = 27,
        km_groundAscendingGain = 28,
        km_groundDescendingGain = 29,
        km_footPlantedGain = 30,
        km_footRaisedGain = 31,
        km_footUnlockGain = 32,
        km_worldFromModelFeedbackGain = 33,
        km_errorUpDownBias = 34,
        km_alignWorldFromModelGain = 35,
        km_hipOrientationGain = 36,
        km_footPlantedAnkleHeightMS = 37,
        km_footRaisedAnkleHeightMS = 38,
        km_maxAnkleHeightMS = 39,
        km_minAnkleHeightMS = 40,
        kFootIKEnable = 41,
        kSpeedDamped = 42,
        kIsBlockHit = 43,
        kiWantBlockFalse = 44,
        kSpeedSampled = 45,
        kstaggerDirection = 46,
        kiGetUpType = 47,
    };

    uint64_t key = 14566708169643289121;
    
    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
        {kbEquipOk,
            kbAnimationDriven,
            kIsAttackReady,
            kbAllowRotation,
            kIsRecoiling,
            kIsStaggering,
            kIsAttacking,
            },
        {kSpeed,
            kTurnDelta,
            kDirection,
            kSpeedSampled,
            },
        {kiSyncIdleLocomotion,
            kiSyncTurnState,
            }));
}
