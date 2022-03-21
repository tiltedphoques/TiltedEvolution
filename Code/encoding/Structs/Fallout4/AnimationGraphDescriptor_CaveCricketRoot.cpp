#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_CaveCricketRoot.h>


AnimationGraphDescriptor_CaveCricketRoot::AnimationGraphDescriptor_CaveCricketRoot(
    AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kiGetUpType = 0,
        kbAnimationDriven = 1,
        kiCombatState = 2,
        kbManualGraphChange = 3,
        kcHitReactionBodyPart = 4,
        kbAllowRotation = 5,
        kbGraphDriven = 6,
        kbGraphDrivenRotation = 7,
        kbGraphDrivenTranslation = 8,
        kiState = 9,
        kiState_CaveCricket = 10,
        kbReactEnabled = 11,
        kbGraphWantsFootIK = 12,
        km_errorOutTranslation = 13,
        km_alignWithGroundRotation = 14,
        km_worldFromModelFeedbackGain = 15,
        kiSyncTurnState = 16,
        kiNonCombatStandingState = 17,
        kiNonCombatLocomotionState = 18,
        kiSyncIdleLocomotion = 19,
        kSpeed = 20,
        kiCombatStandingState = 21,
        kiCombatLocomotionState = 22,
        kIsAttackReady = 23,
        kbEnableAttackMod = 24,
        kDirection = 25,
        kbEquipOk = 26,
        kfRandomClipStartTimePercentage = 27,
        kTurnDeltaSmoothed = 28,
        kbIsSynced = 29,
        kiSyncForwardState = 30,
        kbGraphWantsHeadTracking = 31,
        kiLocomotionSpeed = 32,
        kWalkBackSpeedMult = 33,
        kWalkForwardSpeedMult = 34,
        krunForwardSpeedMult = 35,
        kiDynamicAnimSelector = 36,
        kiRecoilSelector = 37,
        kTurnDelta = 38,
        krunForwardSlowSpeedMult = 39,
        k_DodgeSpeed = 40,
        kbAllowHeadTracking = 41,
    };

    uint64_t key = 7359588577465619653;

    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
            {
                kbManualGraphChange,
                kbEquipOk,
                kIsAttackReady,
                kbAnimationDriven,
            },
            {
                krunForwardSlowSpeedMult,
                kTurnDelta,
                kSpeed,
                kWalkBackSpeedMult,
                kTurnDeltaSmoothed,
                krunForwardSpeedMult,
                kWalkForwardSpeedMult,
                kDirection,
            },
            {
                kiSyncTurnState,
                kiSyncIdleLocomotion,
                kiCombatState,
                kcHitReactionBodyPart,
                kiLocomotionSpeed,
                kiSyncForwardState,
            }));
}
