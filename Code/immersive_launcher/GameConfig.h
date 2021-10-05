#pragma once

#include <cstdint>

constexpr uintptr_t kGenericLoadLimit = 0x140000000 + 0x70000000;

struct GameConfig
{
    const wchar_t* dllClientName;
    const wchar_t* fullGameName;
    uint32_t steamAppId;
    uint32_t loadLimit;
};

#if (1)
static const GameConfig kGame{L"SkyrimTogether.dll", L"Skyrim Special Edition", 489830, kGenericLoadLimit};
#endif
