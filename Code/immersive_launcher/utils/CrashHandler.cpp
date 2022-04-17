#include "CrashHandler.h"

#include <sentry.h>

#include <BuildInfo.h>


void InstallCrashHandler()
{
    sentry_options_t* options = sentry_options_new();

    sentry_options_set_database_path(options, ".sentry-native");

#if defined(TARGET_ST)
    sentry_options_set_dsn(options, "https://96c601d451c94b32adb826aa62c6d50f@o228105.ingest.sentry.io/6269770");
#else
    sentry_options_set_dsn(options, "https://63886f8f9ef54328bc3373b07750a028@o228105.ingest.sentry.io/6273696");
#endif

    sentry_options_set_auto_session_tracking(options, false);
    sentry_options_set_symbolize_stacktraces(options, true);

    sentry_options_set_release(options, BUILD_COMMIT);

    sentry_init(options);
}

void UninstallCrashHandler()
{
    sentry_shutdown();
}
