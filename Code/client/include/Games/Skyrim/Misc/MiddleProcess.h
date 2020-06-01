#pragma once

#if TP_SKYRIM

struct MiddleProcess
{
    // 0xB0 - pitch
#if TP_PLATFORM_64
    uint8_t pad0[0xB8];
#else
    uint8_t pad0[0x68];
#endif
    float direction; // B8 - 68
    // 0xB8 - direction
    //
    // 0x326 - bool lookat 
};

#endif
