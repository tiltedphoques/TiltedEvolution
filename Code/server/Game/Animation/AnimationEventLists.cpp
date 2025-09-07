#include "AnimationEventLists.h"

/*
/* The lists in this file might not contain all relevant animation events, so extend as necessary
*/

const Map<String, String> AnimationEventLists::g_actionsStart = {
    // General actions
    {"moveStart", ""},
    {"bowAttackStart", ""},
    {"blockStart", ""},
    {"staggerStart", ""},
    {"staggerIdleStart", ""},
    {"shoutStart", ""},
    {"SwimStart", ""},
    {"SprintStart", ""},
    {"SneakStart", ""},
    {"torchEquip", ""},
    {"blockHitStart", ""},
    {"bleedOutStart", ""},
    {"blockAnticipateStart", ""},
    {"HorseEnterSwim", ""}, // TODO: HorseEnterInstant?
    {"MountedSwimStart", ""},
    // Idle animations that have Instant counterparts
    {"HorseEnter", "HorseEnterInstant"}, // TODO: Check manually
    {"IdleAlchemyEnter", "IdleAlchemyEnterInstant"},
    {"IdleAltarEnter", "IdleAltarEnterInstant"},
    {"IdleBarCounterEnter", "IdleBarCounterEnterInstant"},
    {"IdleBedEnterStart", "IdleBedEnterInstant"},
    {"IdleBedLeftEnterStart", "IdleBedLeftEnterInstant"},
    {"IdleBedRightEnterStart", "IdleBedRightEnterInstant"},
    {"IdleBedRollFrontEnterStart", "IdleBedRollFrontEnterInstant"},
    {"IdleBedRollLeftEnterStart", "IdleBedRollLeftEnterInstant"},
    {"IdleBedRollRightEnterStart", "IdleBedRollRightEnterInstant"},
    {"IdleBlacksmithForgeEnter", "IdleBlackSmithingEnterInstant"},
    {"IdleBlackSmithingEnterStart", "IdleBlackSmithingEnterInstant"},
    {"IdleChairCHILDFrontEnter", "IdleChairCHILDEnterInstant"},
    {"IdleChairCHILDLeftEnter", "IdleChairCHILDEnterInstant"},
    {"IdleChairCHILDRightEnter", "IdleChairCHILDEnterInstant"},
    {"IdleChildSitOnKnees", "IdleChildSitOnKneesInstant"}, // TODO: exists?
    {"IdleChairEnterStart", "IdleChairEnterInstant"},
    {"IdleChairEnterToSit", "IdleChairEnterInstant"},
    {"IdleChairFrontEnter", "IdleChairEnterInstant"},
    {"IdleChairLeftEnter", "IdleChairEnterInstant"},
    {"IdleChairRightEnter", "IdleChairEnterInstant"},
    {"IdleCookingPotEnter", "IdleCookingPotEnterInstant"},
    {"IdleCookingSpitEnter", "IdleCookingSpitEnterInstant"},
    {"IdleHammerTableEnter", "IdleHammerTableEnterInstant"},
    {"IdleHammerWallEnter", "IdleHammerWallEnterInstant"},
    {"IdleJarlChairEnter", "IdleJarlChairEnterInstant"},
    {"IdleLadderEnter", "IdleLadderEnterInstant"},
    {"IdleLayDownEnter", "IdleLayDownEnterInstant"},
    {"IdleLeanTable", "IdleLeanTableEnterInstant"},
    {"IdleLeanTableEnter", "IdleLeanTableEnterInstant"},
    {"IdlePickaxeEnter", "IdlePickaxeEnterInstant"},
    {"IdleRailLeanEnter", "IdleRailLeanEnterInstant"},
    {"IdleSitCrossLeggedEnter", "IdleSitCrossLeggedEnterInstant"},
    {"IdleSitLedge", "IdleSitLedgeEnterInstant"},
    {"IdleSitLedge_Enter", "IdleSitLedgeEnterInstant"},
    {"IdleSitLedgeEnter", "IdleSitLedgeEnterInstant"},
    {"IdleStoolEnter", "IdleStoolEnterInstant"},
    {"IdleTableEnter", "IdleTableEnterInstant"},
    {"IdleTableEnterPlayer", "IdleTableEnterInstant"},
    {"IdleWallLeanStart", "IdleWallLeanEnterInstant"},
    {"IdleWoodPickUpEnter", "IdleWoodPickUpEnterInstant"},
    {"IdleWoodChopStart", "IdleWoodChopEnterInstant"},
    {"IdleTanningEnter", "IdleTanningEnterInstant"}, // TODO: Check manually
    {"IdleSharpeningWheelStart", "IdleSharpeningWheelEnterInstant"},
    {"IdleSearchingTable", "IdleSearchTableEnterInstant"}, // TODO: Check manually
    {"IdleSearchingChest", "IdleSearchChestEnterInstant"},
    {"IdleMillLoadStart", "IdleMillLoadEnterInstant"},
    {"IdleLeverPushStart", "IdleLeverPushEnterInstant"}, // TODO: Check manually
    {"IdleHideREnter", "IdleHideREnterInstant"}, // TODO: Check manually
    {"IdleHideLEnter", "IdleHideLEnterInstant"}, // TODO: Check manually
    {"IdleEnchantingEnter", "IdleEnchantingEnterInstant"}, // TODO: Check manually
    {"IdleCounterStart", "IdleCounterEnterInstant"},
    {"IdleCartBenchEnter", "IdleCartBenchEnterInstant"},
    {"IdleCarryBucketFillEnter", "IdleCarryBucketFillEnterInstant"},  // TODO: Check manually
    {"IdleCarryBucketPourEnter", "IdleCarryBucketPourEnterInstant"}, // TODO: Check manually
    {"IdleBoundKneesStart", "IdleBoundKneesEnterInstant"},

    {"IdleLeftChairEnterStart", ""}, // TODO: Chair enter instant?
    {"IdleRightChairEnterStart", ""}, // ^
    {"IdlePickaxeTableEnter", ""}, // TODO: IdlePickaxeEnterInstant?
    {"IdlePickaxeFloorEnter", ""}, // ^

    // No instant counterpart exists
    {"IdleWarmHands", ""},
    {"IdleTablePassOutEnter", ""},
    {"IdleTableMugEnter", ""},
    {"IdleTableDrinkEnter", ""},
    {"IdleTableDrinkAndMugEnter", ""},
    {"IdleReadElderScrollStart", ""},
    {"IdleLuteStart", ""},
    {"idleLooseSweepingStart", ""},
    {"LooseSweepingStart", ""},
    {"IdleGetAttention", ""},
    {"IdleFurnitureStart", ""},
    {"IdleFluteStart", ""},
    {"idleEatingStandingStart", ""},
    {"IdleDrumStart", ""},
    {"idleDrinkingStandingStart", ""},
    {"IdleDialogueHappyStart", ""},
    {"IdleDialogueExpressiveStart", ""},
    {"IdleDialogueAngryStart", ""},
    {"IdleCombatStart", ""},
    {"IdleCombatShieldStart", ""},
    {"IdleBeggar", ""},
};

const Set<String> AnimationEventLists::g_actionsExit = {
    {"IdleForceDefaultState"}, // Belongs here too
    {"BleedOutEarlyExit"},
    {"HorseExit"},
    {"IdleStop"},
    {"IdleStopInstant"},
    {"IdleChairExitToStand"},
    {"IdleChairFrontExit"},
    {"idleChairLeftExit"},
    {"idleChairRightExit"},
    {"IdleBedExitToStand"},
    {"IdleCartPrisonerAExit"},
    {"IdleFurnitureExit"},
    {"IdleFurnitureExitSlow"},
    {"IdleLaydown_Exit"},
    {"IdleLayDown_Exit"},
    {"IdleLounge_Exit"},
    {"IdleRailLeanExit"},
    {"IdleSitLedge_Exit"},
    {"IdleStoolBackExit"},
    {"IdleTableBackExit"},
    {"IdleWebExit"},
    {"IdleChairExitStart"},
    {"IdleBedExitStart"},
    {"IdleBedLeftExitStart"},
    {"IdleBedRightExitStart"},
    {"IdleBedRollFrontExitStart"},
    {"IdleBedRollLeftExitStart"},
    {"IdleBedRollRightExitStart"},
    {"IdleChairCHILDFrontExit"},
    {"IdleChairCHILDLeftExit"},
    {"IdleChairCHILDRightExit"},
};

const Set<String> AnimationEventLists::g_actionsIgnore = {
    // Animation events with empty names don't carry anything useful besides 
    // various animvars updates and state changes to the currently running animations.
    // Ignored because they spam too much
    {""},
    {"TurnLeft"},
    {"TurnRight"},
    {"turnStop"},
    {"CyclicFreeze"},
    {"CyclicCrossBlend"},
    {"NPC_BumpedFromBack"},
    {"NPC_BumpedFromLeft"},
    {"NPC_BumpedFromRight"},
    {"NPC_BumpEnd"},
    {"NPC_BumpFromFront"},
    {"NPC_TurnLeft180"},
    {"NPC_TurnLeft90"},
    {"NPC_TurnRight180"},
    {"NPC_TurnRight90"},
    {"NPC_TurnToWalkLeft180"},
    {"NPC_TurnToWalkLeft90"},
    {"NPC_TurnToWalkRight180"},
    {"NPC_TurnToWalkRight90"},
    // Ignore jump animations
    {"JumpDirectionalStart"},
    {"JumpFall"},
    {"JumpFallDirectional"},
    {"JumpFastEnd"},
    {"JumpFastLeft"},
    {"JumpFastRight"},
    {"JumpForwardEnd"},
    {"JumpForwardStart"},
    {"JumpLand"},
    {"JumpLandDirectional"},
    {"JumpLandEnd"},
    {"JumpLandFailSafe"},
    {"JumpLandFast"},
    {"JumpLandSoft"},
    {"JumpSoftEnd"},
    {"JumpStandingStart"},
    // Weapon drawing is handled separately in CharacterService
    {"WeapEquip"},
    {"WeapSoloEquip"},
    {"WeapForceEquip"},
    // There's an elusive bug on the client where it would spam "Unequip"
    // and "combatStanceStop" a lot after changing cells
    {"Unequip"},
    {"combatStanceStop"},
};

const String AnimationEventLists::kIdleForceDefaultState = "IdleForceDefaultState";
