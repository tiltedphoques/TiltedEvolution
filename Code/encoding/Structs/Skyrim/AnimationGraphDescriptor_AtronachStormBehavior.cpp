#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_AtronachStormBehavior.h>

AnimationGraphDescriptor_AtronachStormBehavior::AnimationGraphDescriptor_AtronachStormBehavior(AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kblendDefault = 0,
        kiSyncIdleLocomotion = 1,
        kblendSlow = 2,
        kSpeed = 3,
        kDirection = 4,
        kIsStaggering = 5,
        kIsRecoiling = 6,
        kbEquipOk = 7,
        kiState = 8,
        kiState_AtronachStormDefault = 9,
        kIsAttackReady = 10,
        kbHeadTracking = 11,
        kblendFast = 12,
        kCombatSpeedMult = 13,
        kIsAttacking = 14,
        kiSyncDefaultState = 15,
        kDirectionDamped = 16,
        kSpeedAcc = 17,
        kTimeDelta = 18,
        kbAllowRotation = 19,
        kstaggerMagnitude = 20,
        kbAnimationDriven = 21,
        kbMLh_Ready = 22,
        kisCasting = 23,
        kbWantCastLeft = 24,
        kiLeftHandType = 25,
        kIsSummoned = 26,
        kIsBashing = 27,
        kIsPowerAttacking = 28,
        kstaggerDirection = 29,
        kiCombatStateID = 30,
        kiGetUpType = 31,
    };

    uint64_t key = 9011796343880008240;
    
    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
        {kbEquipOk,
            kbWantCastLeft,
            kbAnimationDriven,
            kIsAttackReady,
            kbMLh_Ready,
            kbWantCastLeft,
            kisCasting,
            kbAllowRotation,
            kIsRecoiling,
            kIsStaggering,
            kIsAttacking,
            },
        {kSpeed,
            kDirection,
            },
        {kiSyncIdleLocomotion,
            kiSyncDefaultState,
            kiCombatStateID,
            }));
}
