#pragma once

#include <array>

struct AnimationData
{
#if TP_SKYRIM
    enum Flags
    {
        kbEquipOk = 0,
        kbMotionDriven,
        kIsBeastRace,
        kIsSneaking,
        kIsBleedingOut,
        kIsCastingDual,
        kIs1HM,
        kIsCastingRight,
        kIsCastingLeft,
        kIsBlockHit,
        kIsPlayer,
        kIsNPC,
        kbIsSynced,
        kbVoiceReady,
        kbWantCastLeft,
        kbWantCastRight,
        kbWantCastVoice,
        kb1HM_MLh_attack,
        kb1HMCombat,
        kbAnimationDriven,
        kbCastReady,
        kbAllowRotation,
        kbMagicDraw,
        kbMLh_Ready,
        kbMRh_Ready,
        kbInMoveState,
        kbSprintOK,
        kbIdlePlaying,
        kbIsDialogueExpressive,
        kbAnimObjectLoaded,
        kbEquipUnequip,
        kbAttached,
        kbIsH2HSolo,
        kbHeadTracking,
        kbIsRiding,
        kbTalkable,
        kbRitualSpellActive,
        kbInJumpState,
        kbHeadTrackSpine,
        kbLeftHandAttack,
        kbIsInMT,
        kbHumanoidFootIKEnable,
        kbHumanoidFootIKDisable,
        kbStaggerPlayerOverride,
        kbNoStagger,
        kbIsStaffLeftCasting,
        kbPerkShieldCharge,
        kbPerkQuickShot,
        kIsAttacking,
        kIsBlocking,
        kIsBashing,
        kIsStaggering,
        kIsRecoiling,
        kIsEquipping,
        kIsUnequipping,
        kIsInFurniture,
        kbNeutralState,
        kiSyncIdleLocomotion,
        kiSyncForwardState,
        kiSyncTurnState,
        kiIsInSneak,
        kiWantBlock,
        kiRegularAttack,
        kbBowDrawn,
        kBooleanCount
    };

    enum Actions
    {
        kRightAttack = 0x13005,
        kJump = 0x13006,
        kLand = 0x937F5,
        kDraw = 0x132AF,
        kRightPowerAttack = 0x13383,
        kRightRelease = 0x13454,
        kActivate = 0x13009,
        kMoveStart = 0x959F8,
        kMoveStop = 0x959F9,
        kFall = 0x937F4,
        kSheath = 0x46BAF,
        kTurnRight = 0x959FC,
        kTurnLeft = 0x959FD,
        kTurnStop = 0x959FE
    };

    enum FloatVariables
    {
        kSpeed = 0,
        kDirection = 1,
        kSpeedSampled = 40,
        kweapAdj = 59,
        kFloatCount = 4
    };

    enum IntegerVariables
    {
        kTurnDelta = 2,
        kiRightHandEquipped = 179,
        kiLeftHandEquipped = 180,
        i1HMState = 77,
        kiState = 94,
        kiLeftHandType = 60,
        kiRightHandType = 35,
        kIntegerCount = 8
    };
#elif TP_FALLOUT

enum Flags
{
    kBooleanCount = 0
};

enum Actions
{
};

enum FloatVariables
{
    kFloatCount = 0
};

enum IntegerVariables
{
    kIntegerCount = 0
};

#endif

    static std::array<uint32_t, kBooleanCount> s_booleanLookUpTable;
    static std::array<uint32_t, kFloatCount> s_floatLookupTable;
    static std::array<uint32_t, kIntegerCount> s_integerLookupTable;
};

