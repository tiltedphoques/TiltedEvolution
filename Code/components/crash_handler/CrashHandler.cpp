#include "CrashHandler.h"

#include <sentry.h>
#include <spdlog/spdlog.h>

#include <BuildInfo.h>

namespace
{
constexpr char kSkyrimServerDSN[] = R"(https://6aff0a6955754bdebfffb064813b9042@o228105.ingest.sentry.io/6303666)";
constexpr char kSkyrimClientDSN[] =  R"(https://96c601d451c94b32adb826aa62c6d50f@o228105.ingest.sentry.io/6269770)";

constexpr char kFalloutServerDSN[] = R"(https://2a3d561652734ca78e539c3fb5219a38@o228105.ingest.sentry.io/6303669)";
constexpr char kFalloutClientDSN[] = R"(https://63886f8f9ef54328bc3373b07750a028@o228105.ingest.sentry.io/6273696)";

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
    vsprintf(rawBuffer, aFormat, aArgList);

    auto level = TranslateSentryLevel(aLogLevel);
    spdlog::default_logger()->log(level, rawBuffer);
}

    // here for future implementation :D
#if 0
    {

        Base::TaskDialog dia(GetModuleHandleW(nullptr), L"SkyrimTogether Crash", L"SkyrimTogether has crashed!",
                             L"Crash information has been collected", L"SkyrimTogether.exe+0x4FEAFF");
        dia.AppendButton(100, L"Send crash report");
        dia.AppendButton(101, L"Ask for help on discord");
        dia.SetDefaultButton(100);

        const int result = dia.Show();
        if (result == 101)
        {
            ShellExecuteW(nullptr, L"open", LR"(https://discord.com/invite/SkyrimTogether)",
                          nullptr, nullptr, SW_SHOWNORMAL);
        }

        exit(4);
    }
#endif

sentry_value_t BeforeSendHandler(sentry_value_t event, void* hint, void* closure)
{
    //MessageBoxW(nullptr, L"BeforeSend", 0, 0);
    return event;
}

bool IsDebugging()
{
#if defined _WIN32
    return ::IsDebuggerPresent();
#endif
    return false;
}
} // namespace

void InstallCrashHandler(bool aServer, bool aSkyrim)
{
    if (IsDebugging())
        return;

    sentry_options_t* options = sentry_options_new();

    sentry_options_set_database_path(options, ".sentry-native");
    if (aSkyrim)
        sentry_options_set_dsn(options, aServer ? kSkyrimServerDSN : kSkyrimClientDSN);
    else
        sentry_options_set_dsn(options, aServer ? kFalloutServerDSN : kFalloutClientDSN);

    sentry_options_set_auto_session_tracking(options, false);
    sentry_options_set_symbolize_stacktraces(options, true);
    //sentry_options_set_before_send(options, BeforeSendHandler, nullptr);

#if 0
    sentry_options_set_debug(options, 1);
    sentry_set_level(sentry_level_t::SENTRY_LEVEL_DEBUG);
#endif

    sentry_options_set_logger(options, SentryLogHandler, nullptr);
    sentry_options_set_release(options, BUILD_COMMIT);

    if (sentry_init(options) != 0)
        spdlog::error("Sentry init failed");
}

void UninstallCrashHandler()
{
    if (IsDebugging())
        return;
    sentry_shutdown();
}
