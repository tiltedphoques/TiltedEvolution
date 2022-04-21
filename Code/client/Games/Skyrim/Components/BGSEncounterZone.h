#pragma once

#include <Forms/TESForm.h>

struct BGSLocation;

struct ENCOUNTER_ZONE_DATA
{
    TESForm* pZoneOwner;
    BGSLocation* pLocation;
    char iOwnerRank;
    uint8_t cMinLevel;
    char cFlags;
    uint8_t cMaxLevel;
};

struct ENCOUNTER_ZONE_GAME_DATA
{
    uint32_t iDetachTime;
    uint32_t iAttachTime;
    uint32_t iResetTime;
    uint16_t sZoneLevel;
};

struct BGSEncounterZone : TESForm
{
    // TODO: this needs to be hooked instead
    void DetermineLevel() noexcept;
    uint16_t GetCalcedZoneLevel() noexcept;

    ENCOUNTER_ZONE_DATA Data;
    ENCOUNTER_ZONE_GAME_DATA GameData;
};

static_assert(sizeof(BGSEncounterZone) == 0x48);

