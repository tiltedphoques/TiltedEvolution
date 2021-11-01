#pragma once

// Libcr validation required
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

}
