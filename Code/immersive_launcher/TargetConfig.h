// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <cstdint>
// How much memory the target may consume during load
// see ExeLoader for more details.
constexpr uintptr_t kGenericLoadLimit = 0x140000000 + 0x70000000;

struct TargetConfig
{
    const wchar_t* dllClientName;
    const wchar_t* fullGameName;
    const wchar_t* shortGameName;
    uint32_t steamAppId;
    uintptr_t loadLimit;
};

// clang-format off

#include <BranchInfo.h>

#if defined(TARGET_ST)
static constexpr TargetConfig CurrentTarget{
    L"SkyrimTogether.dll", 
    L"Skyrim Special Edition", 
    L"Skyrim Special Edition", 
    489830, kGenericLoadLimit};
#elif defined(TARGET_FT)
static constexpr TargetConfig CurrentTarget{
    L"FalloutTogether.dll", 
    L"Fallout4", 
    L"Fallout4", 
    377160, kGenericLoadLimit};
#endif

// clang-format on
