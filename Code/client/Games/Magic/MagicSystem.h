#pragma once

namespace MagicSystem
{

enum CastingSource : int32_t
{
    LEFT_HAND = 0x0,
    RIGHT_HAND = 0x1,
    OTHER = 0x2,
    INSTANT = 0x3,
    CASTING_SOURCE_COUNT = 0x4,
};

enum SoundID : int32_t
{
    ESSID_DRAWSHEATHE_LPM = 0x0,
    ESSID_CHARGE = 0x1,
    ESSID_READY_LOOP = 0x2,
    ESSID_RELEASE = 0x3,
    ESSID_CAST_LOOP = 0x4,
    ESSID_HIT = 0x5,
    ESSID_COUNT = 0x6,
};

enum CannotCastReason : int32_t
{
    NOCAST_OK = 0x0,
    NOCAST_MAGICKA = 0x1,
    NOCAST_POWERUSED = 0x2,
    NOCAST_RANGEDUNDERWATER = 0x3,
    NOCAST_MULTIPLECAST = 0x4,
    NOCAST_ITEM_CHARGE = 0x5,
    NOCAST_CASTWHILESHOUTING = 0x6,
    NOCAST_SHOUTWHILECASTING = 0x7,
    NOCAST_SHOUTWHILERECOVERING = 0x8,
    NOCAST_CUSTOMREASON_START = 0x64,
};

enum CastingType : int32_t
{
    CONSTANT_EFFECT = 0x0,
    FIRE_AND_FORGET = 0x1,
    CONCENTRATION = 0x2,
    // Skyrim: SCROLL = 0x3?
    CASTING_COUNT = 0x3,
};

enum Delivery : int32_t
{
    SELF = 0x0,
    TOUCH = 0x1,
    AIMED = 0x2,
    TARGET_ACTOR = 0x3,
    TARGET_LOCATION = 0x4,
    DELIVERY_COUNT = 0x5,
};

enum SpellType : int32_t
{
    SPELL = 0x0,
    DISEASE = 0x1,
    POWER = 0x2,
    LESSER_POWER = 0x3,
    ABILITY = 0x4,
    POISON = 0x5,
    ENCHANTMENT = 0x6,
    POTION = 0x7,
    WORTCRAFT = 0x8,
    LEVELED_SPELL = 0x9,
    ADDICTION = 0xA,
    VOICE_POWER = 0xB,
    STAFF_ENCHANTMENT = 0xC,
    SCROLL = 0xD,
    SPELL_TYPE_COUNT = 0xE,
};

}

namespace EffectArchetypes
{
enum ArchetypeID : int32_t
{
    kNone = -1,
    kValueModifier = 0,
    kScript = 1,
    kDispel = 2,
    kCureDisease = 3,
    kAbsorb = 4,
    kDualValueModifier = 5,
    kCalm = 6,
    kDemoralize = 7,
    kFrenzy = 8,
    kDisarm = 9,
    kCommandSummoned = 10,
    kInvisibility = 11,
    kLight = 12,
    kDarkness = 13,
    kNightEye = 14,
    kLock = 15,
    kOpen = 16,
    kBoundWeapon = 17,
    kSummonCreature = 18,
    kDetectLife = 19,
    kTelekinesis = 20,
    kParalysis = 21,
    kReanimate = 22,
    kSoulTrap = 23,
    kTurnUndead = 24,
    kGuide = 25,
    kWerewolfFeed = 26,
    kCureParalysis = 27,
    kCureAddiction = 28,
    kCurePoison = 29,
    kConcussion = 30,
    kValueAndParts = 31,
    kAccumulateMagnitude = 32,
    kStagger = 33,
    kPeakValueModifier = 34,
    kCloak = 35,
    kWerewolf = 36,
    kSlowTime = 37,
    kRally = 38,
    kEnhanceWeapon = 39,
    kSpawnHazard = 40,
    kEtherealize = 41,
    kBanish = 42,
    kSpawnScriptedRef = 43,
    kDisguise = 44,
    kGrabActor = 45,
    kVampireLord = 46
};
}
