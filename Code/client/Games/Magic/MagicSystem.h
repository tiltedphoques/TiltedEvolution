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

}

namespace EffectArchetypes
{
// These are from fallout 4, need to be validated for skyrim
enum ArchetypeID : int32_t
{
    ARCHETYPE_NONE = -1,
    VALUE_MODIFIER = 0x0,
    SCRIPT = 0x1,
    DISPEL = 0x2,
    CURE_DISEASE = 0x3,
    ABSORB = 0x4,
    DUAL_VALUE_MODIFIER = 0x5,
    CALM = 0x6,
    DEMORALIZE = 0x7,
    FRENZY = 0x8,
    DISARM = 0x9,
    COMMAND_SUMMONED = 0xA,
    INVISIBILITY = 0xB,
    LIGHT = 0xC,
    DARKNESS = 0xD,
    NIGHT_EYE = 0xE,
    LOCK = 0xF,
    OPEN = 0x10,
    BOUND_WEAPON = 0x11,
    SUMMON_CREATURE = 0x12,
    DETECT_LIFE = 0x13,
    TELEKINESIS = 0x14,
    PARALYZE = 0x15,
    REANIMATE = 0x16,
    SOUL_TRAP = 0x17,
    TURN_UNDEAD = 0x18,
    GUIDE = 0x19,
    UNUSED_01 = 0x1A,
    CURE_PARALYSIS = 0x1B,
    CURE_ADDICTION = 0x1C,
    CURE_POISON = 0x1D,
    CONCUSSION = 0x1E,
    STIMPAK = 0x1F,
    ACCUMULATING_MAGNITUDE = 0x20,
    STAGGER = 0x21,
    PEAK_VALUE_MODIFIER = 0x22,
    CLOAK = 0x23,
    UNUSED_02 = 0x24,
    SLOW_TIME = 0x25,
    RALLY = 0x26,
    ENHANCE_WEAPON = 0x27,
    SPAWN_HAZARD = 0x28,
    ETHEREALIZE = 0x29,
    BANISH = 0x2A,
    SPAWN_SCRIPTED_REF = 0x2B,
    DISGUISE = 0x2C,
    DAMAGE = 0x2D,
    IMMUNITY = 0x2E,
    PERMANENT_REANIMATE = 0x2F,
    JETPACK = 0x30,
    CHAMELEON = 0x31,
    ARCHETYPE_COUNT = 0x32,
};
}
