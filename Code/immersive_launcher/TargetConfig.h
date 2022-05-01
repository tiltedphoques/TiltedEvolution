// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <cstdint>
#include <limits>
#include <BuildInfo.h>

#define CLIENT_DLL 0

struct TargetConfig
{
    const wchar_t* dllClientName;
    const wchar_t* fullGameName;
    uint32_t steamAppId;
    uint32_t exeLoadSz;
    // Needs to be kept up to date.
    uint32_t exeDiskSz;
};

// clang-format off

#if defined(TARGET_ST)
static constexpr TargetConfig CurrentTarget{
    L"SkyrimTogether.dll", 
    L"Skyrim Special Edition", 
    489830, 0x40000000, 35410264};
#define TARGET_NAME L"SkyrimSE"
#define PRODUCT_NAME L"Skyrim Together"
#define SHORT_NAME L"Skyrim Special Edition"
#elif defined(TARGET_FT)
static constexpr TargetConfig CurrentTarget{
    L"FalloutTogether.dll", 
    L"Fallout4", 
    377160, 0x70000000, std::numeric_limits<std::uint32_t>::max()};
#define TARGET_NAME L"Fallout4"
#define PRODUCT_NAME L"Fallout Together"
#define SHORT_NAME L"Fallout4"
#endif

// clang-format on
