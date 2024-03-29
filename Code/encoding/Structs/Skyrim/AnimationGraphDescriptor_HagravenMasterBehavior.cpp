#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_HagravenMasterBehavior.h>

AnimationGraphDescriptor_HagravenMasterBehavior::AnimationGraphDescriptor_HagravenMasterBehavior(AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kSpeed = 0,
        kIsStaggering = 1,
        kIsAttacking = 2,
        kIsRecoiling = 3,
        kDirection = 4,
        kSpeedSampled = 5,
        kiSyncDefaultState = 6,
        kiSyncIdleLocomotion = 7,
        kiState = 8,
        kiState_HagravenDefault = 9,
        kTurnDelta = 10,
        kturnSpeedMult = 11,
        kiSyncTurnState = 12,
        kiRightHandType = 13,
        kbWeapReady = 14,
        kiEquippedItemState = 15,
        kIsAttackReady = 16,
        kbMLh_Ready = 17,
        kbMRh_Ready = 18,
        kIntDirection = 19,
        kblendFast = 20,
        kblendDefault = 21,
        kisCasting = 22,
        kbWantCastRight = 23,
        kbWantCastLeft = 24,
        kbHeadTrackingOn = 25,
        kTargetLocation = 26,
        kcamerafromx = 27,
        kcamerafromy = 28,
        kcamerafromz = 29,
        kbCanHeadTrack = 30,
        kbHeadTracking = 31,
        kbEquipOk = 32,
        kbAnimationDriven = 33,
        kstaggerMagnitude = 34,
        kbIsSynced = 35,
        kiGetUpType = 36,
        kstaggerDirection = 37,
        kTimeStep = 38,
        kbFullyMotionDriven = 39,
        kDirectionDamped = 40,
        kbFootIK = 41,
        kbNoFootIK = 42,
        kIsShouting = 43,
        kIsBusy = 44,
        kIsIdle = 45,
        kbVoiceReady = 46,
        kbWantCastVoice = 47,
        kiCombat = 48,
        kTweenEntryDirection = 49,
        kiDirectionForward = 50,
        kPhonemeBlendTarget = 51,
        kLipBigAah = 52,
        kLipDST = 53,
        kLipEee = 54,
        kLipFV = 55,
        kLipK = 56,
        kLipL = 57,
        kLipR = 58,
        kLipTh = 59,
        kPhonemeBlend = 60,
        kLipAcc = 61,
        kInvPhonemeBlend = 62,
        kbLookAtTarget = 63,
        kbCanLookAtTarget = 64,
        km_worldFromModelFeedbackGain = 65,
        km_alignWorldFromModelGain = 66,
        km_ankeOrientationGain = 67,
        km_frontFootPlantedAnkleHeightMS = 68,
        km_frontFootRaisedAnkleHeightMS = 69,
        km_frontMaxAnkleHeightMS = 70,
        km_frontMinAnkleHeightMS = 71,
        km_rearFootPlantedAnkleHeightMS = 72,
        km_rearFootRaisedAnkleHeightMS = 73,
        km_rearMaxAnkleHeightMS = 74,
        km_rearMinAnkleHeightMS = 75,
        km_rearMaxAnkleAngleDegrees = 76,
        km_raycastDistanceUp = 77,
        km_raycastDistanceDown = 78,
        km_errorOut = 79,
        km_errorOutTranslation = 80,
        km_alignWithGroundRotation = 81,
        kBSLookAtModifier_m_onGain = 82,
        kBSLookAtModifier_m_offGain = 83,
        kLookAtHeadingMaxAngle = 84,
        kBSLookAtModifier_CanLookOutsideLimit = 85,
        kLookAtOutOfRange = 86,
        kbSpeedSynced = 87,
        kdefaultBlend = 88,
        kHasTweenSpeed = 89,
        kTweenSpeed = 90,
        kDistToGoal = 91,
        kTurnDeltaDamped = 92,
        kPitchDeltaDamped = 93,
        kFlightPitchBlend = 94,
        kPathAngle = 95,
        kMovementDirection = 96,
        kMoveDirZ = 97,
        kBSLookAtModifier_m_onGain_Shouting = 98,
        kBSLookAtModifier_m_onGain_Combat = 99,
        kBSLookAtModifier_m_offGain_Shouting = 100,
        kBSLookAtModifier_m_offGain_Combat = 101,
        kPitchDelta = 102,
        kMaxSpeedCurrent = 103,
        kMaxSpeed = 104,
        kMinSpeed = 105,
        kMaxSpeedDamped = 106,
        kPathAngleThreshold = 107,
        kTargetSpeedDamped = 108,
        kTargetSpeed = 109,
        kTweenSpeedDamped = 110,
        kTweenSpeedMin = 111,
        kMaxAcc = 112,
        kTargetSpeedMaxScale = 113,
        kMaxDec = 114,
        kDrag = 115,
        kTargetSpeedThresholdMin = 116,
        kTargetSpeedThresholdMax = 117,
        kIsFeathering = 118,
        kTurnDeltaTarget = 119,
        kTurnDeltaScale = 120,
        kTurnDeltaDampedGain = 121,
        kPitchDeltaTarget = 122,
        kPitchDeltaDampedGain = 123,
        kFlightPitchBlendTarget = 124,
        kFlightPitchBlendGain = 125,
        kPickNewIdleTime = 126,
        kFlightHitInjuredSpeedMin = 127,
        kFlightHitInjuredDecGain = 128,
        kTurnMin = 129,
        kIsTurningLeft = 130,
        kIsTurningRight = 131,
        kfMinSpeed = 132,
        kisMoving = 133,
        kIsMovingForward = 134,
        kIsMovingBackward = 135,
        kBSLookAtModifier_m_onGain_Default = 136,
        kBSLookAtModifier_m_offGain_Default = 137,
        kTweenRotation = 138,
        kbAllowRotation = 139,
        kTweenPosition = 140,
        kConstraintOffset = 141,
        kbTweenUpdate = 142,
        kTweenEntryDirectionFlightKillGrab = 143,
        kTweenEntryDirectionPrev = 144,
        kTweenEntryDirectionCur = 145,
        kLipBigAahDamped = 146,
        kLipGain = 147,
        kLipDSTDamped = 148,
        kLipEeeDamped = 149,
        kLipFVDamped = 150,
        kLipKDamped = 151,
        kLipLDamped = 152,
        kLipRDamped = 153,
        kLipThDamped = 154,
    };

    uint64_t key = 3017922126943190855;

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
                kbWeapReady,
                kisCasting,
                kbCanHeadTrack,
                kbWantCastLeft,
                kbMLh_Ready,
            },
            {
                kSpeed,
                kTurnDelta,
                kDirection,
                kSpeedSampled,
                kturnSpeedMult,
            },
            {
                kiSyncDefaultState,
                kiSyncIdleLocomotion,
                kiSyncTurnState,
            }));
}
