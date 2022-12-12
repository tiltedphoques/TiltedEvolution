// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "Logging.h"
#include "APIBoundary.h"

namespace PluginAPI
{
void PluginAPI_WriteLog(const LogLevel aLogLevel, const char* apFormat, ...)
{
    using namespace spdlog::level;

    level_enum level;
    switch (aLogLevel)
    {
    case LogLevel::kInfo:
    default: level = level_enum::info; break;
    case LogLevel::kWarn: level = level_enum::warn; break;
    case LogLevel::kError: level = level_enum::err; break;
    case LogLevel::kDebug: level = level_enum::debug; break;
    }

    va_list list;
    va_start(list, apFormat);
    char buf[256]{};
    vsprintf(buf, apFormat, list);

    spdlog::log(level, buf);
    va_end(list);
}
} // namespace PluginAPI
