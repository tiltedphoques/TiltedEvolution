#pragma once

namespace CrashReporter
{
void InstallCrashHandler(const char *acBasePath, const char *acReleaseTag);
void UnInstallCrashHandler();

void OnException();
}
