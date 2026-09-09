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

// clang-format off

static constexpr TargetConfig CurrentTarget{ L"Skyrim Special Edition", 489830, 0x40000000 };
#define TARGET_NAME L"SkyrimSE"
#define TARGET_NAME_A "SkyrimSE"
#define PRODUCT_NAME L"Skyrim Together"
#define SHORT_NAME L"Skyrim Special Edition"

// clang-format on
