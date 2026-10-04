#pragma once

#include "ExtraData.h"

#include <Components/TESActorBaseData.h>

struct TESFaction;

struct ExtraFactionChanges : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::Faction;

    bool removeCrimeFaction;                          // 18
    GameArray<TESActorBaseData::FactionRank> entries; // 20
    TESFaction* crimeFaction;                         // 38
};

static_assert(offsetof(ExtraFactionChanges, entries) == 0x20);
static_assert(sizeof(ExtraFactionChanges) == 0x40);
