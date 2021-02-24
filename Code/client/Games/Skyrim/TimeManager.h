#pragma once

#include <Forms/TESGlobal.h>

struct TimeData
{
    static TimeData* Get() noexcept;

    char pad0[0x8];             // 0x0000
    TESGlobal *GameYear;        // 0x0008
    TESGlobal *GameMonth;       // 0x0010
    TESGlobal *GameDay;         // 0x0018
    TESGlobal *GameHour;        // 0x0020
    TESGlobal *GameDaysPassed;  // 0x0028
    TESGlobal *TimeScale;       // 0x0030
    float unk1;                 // 0x0038
    float timeTick;             // 0x003C
    char pad_40[0x88];          // 0x0040
};
