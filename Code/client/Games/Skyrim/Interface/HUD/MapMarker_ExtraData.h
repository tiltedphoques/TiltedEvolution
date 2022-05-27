#pragma once

#include <Components/TESFullName.h>

struct MapMarkerData
{
    static MapMarkerData* New() noexcept;

    enum class Flag : uint8_t
    {
        NONE = 0,
        VISIBLE = 1,
    };

    enum class Type : uint16_t
    {
        kNone = 0,
        kCity = 1,
        kTown = 2,
        kSettlement = 3,
        kCave = 4,
        kCamp = 5,
        kFort = 6,
        kNordicRuins = 7,
        kDwemerRuin = 8,
        kShipwreck = 9,
        kGrove = 10,
        kLandmark = 11,
        kDragonLair = 12,
        kFarm = 13,
        kWoodMill = 14,
        kMine = 15,
        kImperialCamp = 16,
        kStormcloakCamp = 17,
        kDoomstone = 18,
        kWheatMill = 19,
        kSmelter = 20,
        kStable = 21,
        kImperialTower = 22,
        kClearing = 23,
        kPass = 24,
        kAlter = 25,
        kRock = 26,
        kLighthouse = 27,
        kOrcStronghold = 28,
        kGiantCamp = 29,
        kShack = 30,
        kNordicTower = 31,
        kNordicDwelling = 32,
        kDocks = 33,
        kShrine = 34,
        kRiftenCastle = 35,
        kRiftenCapitol = 36,
        kWindhelmCastle = 37,
        kWindhelmCapitol = 38,
        kWhiterunCastle = 39,
        kWhiterunCapitol = 40,
        kSolitudeCastle = 41,
        kSolitudeCapitol = 42,
        kMarkarthCastle = 43,
        kMarkarthCapitol = 44,
        kWinterholdCastle = 45,
        kWinterholdCapitol = 46,
        kMorthalCastle = 47,
        kMorthalCapitol = 48,
        kFalkreathCastle = 49,
        kFalkreathCapitol = 50,
        kDawnstarCastle = 51,
        kDawnstarCapitol = 52,
        kDLC02_TempleOfMiraak = 53,
        kDLC02_RavenRock = 54,
        kDLC02_BeastStone = 55,
        kDLC02_TelMithryn = 56,
        kDLC02_ToSkyrim = 57,
        kDLC02_ToSolstheim = 58,
        // see https://i.imgur.com/L1mWPhR.jpeg
        kCustomMarker = 65,
        kMousePointer = 66
    };

    TESFullName name;
    Flag cFlags;
    Flag cOriginalFlags;
    Type sType;
};

static_assert(offsetof(MapMarkerData, MapMarkerData::cFlags) == 16);
static_assert(offsetof(MapMarkerData, MapMarkerData::sType) == 18);
static_assert(sizeof(MapMarkerData) == 24);
