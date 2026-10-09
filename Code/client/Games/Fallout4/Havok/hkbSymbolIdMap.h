#pragma once

struct hkbSymbolIdMap
{
    virtual ~hkbSymbolIdMap();

    uint8_t pad8[0x20 - 0x8];
    void* pointer20;
};

static_assert(offsetof(hkbSymbolIdMap, pointer20) == 0x20);
