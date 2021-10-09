// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <cstdint>
#include <BranchInfo.h>

#define CLIENT_DLL 0

struct TargetConfig
{
    const wchar_t* dllClientName;
    const wchar_t* fullGameName;
    const wchar_t* shortGameName;
    uint32_t steamAppId;
    uint32_t exeSize;
};

// clang-format off

#if defined(TARGET_ST)
static constexpr TargetConfig CurrentTarget{
    L"SkyrimTogether.dll", 
    L"Skyrim Special Edition", 
    L"Skyrim Special Edition", 
    489830, 0x40000000};
#elif defined(TARGET_FT)
static constexpr TargetConfig CurrentTarget{
    L"FalloutTogether.dll", 
    L"Fallout4", 
    L"Fallout4", 
    377160, 0x70000000};
#endif

// clang-format on
