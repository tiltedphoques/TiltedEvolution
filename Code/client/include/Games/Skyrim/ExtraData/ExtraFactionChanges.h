#pragma once

#ifdef TP_SKYRIM

#include <Games/Skyrim/ExtraData.h>

struct TESFaction;

struct ExtraFactionChanges : BSExtraData
{
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

#endif
