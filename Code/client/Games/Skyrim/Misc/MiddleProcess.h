#pragma once

struct MiddleProcess
{
    // 0xB0 - pitch
    uint8_t pad0[0xB8];
    float direction; // B8
    uint8_t padBC[0x220 - 0xBC];
    TESForm** leftEquippedObject;
    uint8_t pad228[0x260 - 0x228];
    TESForm** rightEquippedObject;
    TESForm** pAmmo;
    // 0xB8 - direction
    //
    // 0x326 - bool lookat 
};

static_assert(offsetof(MiddleProcess, direction) == 0xB8);
static_assert(offsetof(MiddleProcess, leftEquippedObject) == 0x220);
static_assert(offsetof(MiddleProcess, rightEquippedObject) == 0x260);
