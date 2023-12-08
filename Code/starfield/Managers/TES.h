#pragma once

struct TESWorldSpace;

struct TES
{
    static TES* Get()
    {
        return (TES*)0x145905690; // 881024
    }

    virtual void unk0();
    // ...

    uint8_t unk8[0xC4 - 0x8];
    int32_t centerGridX;
    int32_t centerGridY;
    int32_t currentGridX;
    int32_t currentGridY;
    uint8_t unkD4[0x188 - 0xD4];
    TESWorldSpace* pWorldSpace;
    // ...

};

static_assert(offsetof(TES, centerGridX) == 0xC4);
static_assert(offsetof(TES, pWorldSpace) == 0x188);
