#pragma once

struct hkbSymbolIdMap;

// Real name unknown
struct CharacterContext
{
    uint8_t pad0[0x18];
    hkbSymbolIdMap* symbolIdMap; // 18
    uint8_t byte20; // 20
};

static_assert(offsetof(CharacterContext, symbolIdMap) == 0x18);
static_assert(offsetof(CharacterContext, byte20) == 0x20);
