// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "Logging.h"

SERVER_API void PluginAPI_WriteLog(const LogLevel aLogLevel, const char* apFormat, ...)
{
    using namespace spdlog::level;

    level_enum level;
    switch (aLogLevel)
    {
    case LogLevel::kDebug:
        level = level_enum::debug;
        break;
    case LogLevel::kInfo:
    default:
        level = level_enum::info;
        break;
    }

    spdlog::log(level, apFormat);
}
