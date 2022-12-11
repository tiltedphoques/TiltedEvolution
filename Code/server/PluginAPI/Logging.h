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
    kError,
    kDebug
};

// core imp
SERVER_API void PluginAPI_WriteLog(const LogLevel aLogLevel, const char* apFormat, ...);

#define PLUGINAPI_LOG_INFO(...) PluginAPI_WriteLog(LogLevel::kInfo, __VA_ARGS__)
#define PLUGINAPI_LOG_ERROR(...) PluginAPI_WriteLog(LogLevel::kError, __VA_ARGS__)
}
