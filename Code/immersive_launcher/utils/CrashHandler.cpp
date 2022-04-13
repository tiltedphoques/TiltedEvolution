#include "CrashHandler.h"

#include <strsafe.h>
#include <sstream>

#include "client/crashpad_client.h"
#include "client/settings.h"
#include "client/crash_report_database.h"

#include <BuildInfo.h>


using namespace crashpad;

static CrashpadClient client;

LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    if (pExceptionInfo->ExceptionRecord->ExceptionCode == 0xC0000005)
    {
        client.DumpAndCrash(pExceptionInfo);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}


void InstallCrashHandler()
{
    base::FilePath database(L"crashpad");
    std::unique_ptr<CrashReportDatabase> db = CrashReportDatabase::Initialize(database);

#if 1
    if (db != nullptr && db->GetSettings() != nullptr)
    {
        db->GetSettings()->SetUploadsEnabled(true);
    }
#endif

    // Path to the out-of-process handler executable
    base::FilePath handler(L"crashpad_handler.exe");
    // URL used to submit minidumps to
#if defined(TARGET_ST)
    std::string url("https://o228105.ingest.sentry.io/api/6269770/minidump/?sentry_key=96c601d451c94b32adb826aa62c6d50f");
#else
    std::string url("https://o228105.ingest.sentry.io/api/6273696/minidump/?sentry_key=63886f8f9ef54328bc3373b07750a028");
#endif
    // Optional annotations passed via --annotations to the handler
    std::map<std::string, std::string> annotations;

    annotations["commit"] = BUILD_COMMIT;
    annotations["branch"] = BUILD_BRANCH;

    // Optional arguments to pass to the handler
    std::vector<std::string> arguments;

    if (bool success = client.StartHandler(handler, database, database, url, annotations, arguments, true, false))
    {
        success = client.WaitForHandlerStart(INFINITE);
    }

    AddVectoredExceptionHandler(1, &VectoredExceptionHandler);
}
