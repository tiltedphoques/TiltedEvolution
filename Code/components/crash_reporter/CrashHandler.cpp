
#include "CrashHandler.h"

#include <sentry.h>
#include <spdlog/spdlog.h>
#include <string>

namespace CrashReporter
{
namespace
{
spdlog::level::level_enum TranslateSentryLevel(sentry_level_t aSentryLevel)
{
    using ll = spdlog::level::level_enum;

    switch (aSentryLevel)
    {
    case SENTRY_LEVEL_DEBUG:
        return ll::debug;
    case SENTRY_LEVEL_INFO:
        return ll::info;
    case SENTRY_LEVEL_WARNING:
        return ll::warn;
    case SENTRY_LEVEL_ERROR:
        return ll::err;
    case SENTRY_LEVEL_FATAL:
        return ll::critical;
    default:
        return ll::trace;
    }
}

void SentryLogHandler(sentry_level_t aLogLevel, const char* aFormat, va_list aArgList, void* apUserPointer)
{
    // this also null terminates.
    char rawBuffer[1024]{'\n'};
    vsprintf_s(rawBuffer, aFormat, aArgList);

    auto level = TranslateSentryLevel(aLogLevel);
    spdlog::default_logger()->log(level, rawBuffer);
}
} // namespace

bool InstallCrashHandler(const Settings& aConfig)
{
    // Do have user consnt check?
    // sentry_options_set_require_user_consent

    sentry_options_t* options = sentry_options_new();
    sentry_options_set_auto_session_tracking(options, false);
    sentry_options_set_symbolize_stacktraces(options, true);

    // configure the together endpoint.
    sentry_options_set_dsn(options, aConfig.acDSN);
    sentry_options_set_release(options, aConfig.acReleaseTag);

    // TODO: properly set this based on defines present in Force:misc_fixes
    // sentry_options_set_environment(options, "development");

    // again, dependant on our SDK passin options.
    sentry_options_set_debug(options, 1);
    sentry_set_level(sentry_level_t::SENTRY_LEVEL_DEBUG);
    sentry_options_set_logger(options, SentryLogHandler, nullptr);

    std::string crashHandlerPath = std::string(aConfig.acBasePath) + "//crashpad_handler.exe";
    sentry_options_set_handler_path(options, crashHandlerPath.c_str());

    return sentry_init(options) == 0;
}

void UnInstallCrashHandler()
{
    sentry_shutdown();
}

void OnException()
{
}
} // namespace CrashReporter
