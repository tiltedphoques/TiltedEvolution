#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_FrostbiteSpider.h>


AnimationGraphDescriptor_FrostbiteSpider::AnimationGraphDescriptor_FrostbiteSpider(AnimationGraphDescriptorManager& aManager)
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
        kbAllowRotation = 10,
        kturnSpeedMult = 11,
        kiCombatStance = 12,
        kiSyncTurnState = 13,
        kIntDirection = 14,
        kbMotionDriven = 15,
        kiSyncIdleLocomotion = 16,
        kfMinTurnDelta = 17,
        kbAnimationDriven = 18,
        kIsAttacking = 19,
        kiLeftHandType = 20,
        kiWeapCategory = 21,
        kbWantCastLeft = 22,
        kbMLh_Ready = 23,
        kiState_CombatSpider_MT = 24,
        kiState_DefaultSpider_MT = 25,
        kiState = 26,
        kIsBashing = 27,
        kbEquipOk = 28,
        kIsStaggering = 29,
        kIsRecoiling = 30,
        kSpeedDamped = 31,
        kSpeedSampled = 32,
        kiCurrentStateID = 33,
        kiGetUpType = 34,
        kbIsSynced = 35,
    };

    uint64_t key = 1928879069472700161;

    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
        {kbAnimationDriven,
            kbAllowRotation,
            kIsRecoiling,
            kIsStaggering,
            kIsAttacking,
            kbMLh_Ready,
            kbEquipOk,
            kIsAttackReady,
            kbWantCastLeft,
            },
        {kSpeed,
            kTurnDelta,
            kturnSpeedMult,
            kDirection,
            kSpeedSampled,
            },
        {kiLeftHandType,
            kiSyncIdleLocomotion,
            kiSyncTurnState,
            kiCombatStance,
            kiCurrentStateID,
            }));
}
