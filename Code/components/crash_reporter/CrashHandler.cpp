
#include "CrashHandler.h"

#include <sentry.h>

namespace CrashReporter
{
constexpr char kTogetherDSNURL[] = "https://96c601d451c94b32adb826aa62c6d50f@o228105.ingest.sentry.io/6269770";

void InstallCrashHandler(const char* acBasePath, const char* acReleaseTag)
{
    // Do have user consnt check?

    sentry_options_t* options = sentry_options_new();
    sentry_options_set_auto_session_tracking(options, false);
    sentry_options_set_symbolize_stacktraces(options, true);

    // configure the together endpoint.
    sentry_options_set_dsn(options, kTogetherDSNURL);
    sentry_options_set_release(options, acReleaseTag /*"my-project-name@2.3.12"*/);

    // TODO: properly set this based on defines present in Force:misc_fixes
    sentry_options_set_environment(options, "development");

    // again, dependant on our SDK passin options.
    // sentry_options_set_debug(options, 1);

    // acBasePath concat...
    //sentry_options_set_handler_path(options, )


    if (sentry_init(options) != 0)
    {
        return;
    }
}

void UnInstallCrashHandler()
{
    sentry_shutdown();
}

void OnException()
{

}
} // namespace CrashReporter
