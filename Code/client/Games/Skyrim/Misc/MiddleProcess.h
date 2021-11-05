#pragma once

struct ActiveEffect;
struct BGSLoadFormBuffer;

struct MiddleProcess
{
    //void SaveActiveEffects()
    void LoadActiveEffects(BGSLoadFormBuffer* apLoadGameBuffer);

    // 0xB0 - pitch
    uint8_t pad0[0xB8];
    float direction; // B8
    uint8_t padBC[0x1A0 - 0xBC];
    GameList<ActiveEffect*>* ActiveEffects;
    uint8_t pad1A8[0x220 - 0x1A8];
    TESForm** leftEquippedObject;
    uint8_t pad228[0x260 - 0x228];
    TESForm** rightEquippedObject;
    TESForm* pAmmo; // TESAmmo* pAmmo;
    // 0xB8 - direction
    //
    // 0x326 - bool lookat 
};

static_assert(offsetof(MiddleProcess, direction) == 0xB8);
static_assert(offsetof(MiddleProcess, leftEquippedObject) == 0x220);
static_assert(offsetof(MiddleProcess, rightEquippedObject) == 0x260);
