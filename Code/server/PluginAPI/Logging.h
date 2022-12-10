// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include "PluginAPI/PluginAPI.h"

namespace PluginAPI
{
// utilities for sending data to GS
enum class LogLevel
{
    kInfo,
    kDebug
};

// core imp
SERVER_API void PluginAPI_WriteLog(const LogLevel aLogLevel, const char* apFormat, ...);
}
