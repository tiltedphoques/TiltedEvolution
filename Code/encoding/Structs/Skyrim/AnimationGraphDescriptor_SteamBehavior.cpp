#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_SteamBehavior.h>



AnimationGraphDescriptor_SteamBehavior::AnimationGraphDescriptor_SteamBehavior(AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kcurrentDefaultState = 0,
        kiSyncIdleLocomotion = 1,
        kiSyncTurnState = 2,
        kTurnDelta = 3,
        kiState = 4,
        kiState_SteamDefault = 5,
        kDirection = 6,
        kSampledSpeed = 7,
        kSpeed = 8,
        kiLeftHandType = 9,
        kiRightHandType = 10,
        kbAnimationDriven = 11,
        kIsRecoiling = 12,
        kIsStaggering = 13,
        kIsAttacking = 14,
        kbAllowRotation = 15,
        kbEquipOk = 16,
        kIsEquipping = 17,
        kIsUnequipping = 18,
        kbWeapReady = 19,
        kIsAttackReady = 20,
        kiState_SteamCombat = 21,
        kIsBlocking = 22,
        kstaggerMagnitude = 23,
        kIsBashing = 24,
        kbAttached = 25,
        kisCasting = 26,
        kbWantCastVoice = 27,
        kbMLh_Ready = 28,
        kInt32Variable = 29,
        kstaggerDirection = 30,
        kIsShouting = 31,
        kiGetUpType = 32,
        kbVoiceReady = 33,
        kbHeadTrackingOn = 34,
        kTargetLocation = 35,
        kcamerafromx = 36,
        kcamerafromy = 37,
        kcamerafromz = 38,
        kbCastReady = 39,
        kbIsSynced = 40,
    };

    uint64_t key = 12323911819758128165;
    
    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
        {kbEquipOk,
            kbAnimationDriven,
            kIsAttackReady,
            kbAllowRotation,
            kIsRecoiling,
            kIsStaggering,
            kIsAttacking,
            kbVoiceReady,
            kbWeapReady,
            kIsShouting,
            kbAttached,
            kbWantCastVoice,
            },
        {kTurnDelta,
            kSpeed,
            kSampledSpeed,
            kDirection,
            },
        {kcurrentDefaultState,
            kiSyncIdleLocomotion,
            kiSyncTurnState,
            }));
}
