#pragma once

#include <ExtraData.h>

struct TESFaction;

struct ExtraFactionChanges : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::Faction;

    virtual ~ExtraFactionChanges();

    struct Entry
    {
        TESFaction* faction;
        int8_t rank;
    };

    GameArray<Entry> entries;
};

static_assert(sizeof(ExtraFactionChanges::Entry) == 0x10);
static_assert(offsetof(ExtraFactionChanges, entries) == 0x10);
