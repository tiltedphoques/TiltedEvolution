// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include "PluginAPI/PluginAPI.h"

// utilities for sending data to GS
enum class LogLevel
{
    kInfo,
    kDebug
};
SERVER_API void PluginAPI_WriteLog(const LogLevel aLogLevel, const char* apFormat, ...);
