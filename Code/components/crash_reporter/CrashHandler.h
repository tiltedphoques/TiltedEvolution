#pragma once

namespace CrashReporter
{
struct Settings
{
    const char* acBasePath;
    const char* acReleaseTag;
    const char* acEnvironment;
    const char* acDSN;
};
bool InstallCrashHandler(const Settings& aConfig);
void UnInstallCrashHandler();

void OnException();
}
