// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <cstdint>
#include <limits>
#include <BuildInfo.h>

#define CLIENT_DLL 0

struct TargetConfig
{
    const wchar_t* fullGameName;
    uint32_t steamAppId;
    uint32_t exeLoadSz;
};

// One target per build, selected with xmake --game=skyrim|fallout4.
// The client links against exactly one game's engine layout, so a
// compile-time switch is the only sane shape for this.

#if defined(TP_FALLOUT4)

// clang-format off
static constexpr TargetConfig CurrentTarget{ L"Fallout 4", 377160, 0x40000000 };
#define TARGET_NAME L"Fallout4"
#define TARGET_NAME_A "Fallout4"
#define PRODUCT_NAME L"Fallout 4 Together"
#define SHORT_NAME L"Fallout 4"
#define SUPPORTED_GAME_VERSION "1.11.240.0"
// clang-format on

#else

// clang-format off
static constexpr TargetConfig CurrentTarget{ L"Skyrim Special Edition", 489830, 0x40000000 };
#define TARGET_NAME L"SkyrimSE"
#define TARGET_NAME_A "SkyrimSE"
#define PRODUCT_NAME L"Skyrim Together"
#define SHORT_NAME L"Skyrim Special Edition"
#define SUPPORTED_GAME_VERSION "1.7.104.0"
// clang-format on

#endif
