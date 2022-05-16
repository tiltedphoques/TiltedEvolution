#pragma once

void InstallCrashHandler(bool aServer, bool aSkyrim = true);
void UninstallCrashHandler();

struct ScopedCrashHandler
{
    ScopedCrashHandler(bool aServer = false, bool aSkyrim = true)
    {
        InstallCrashHandler(aServer, aSkyrim);
    }

    ~ScopedCrashHandler()
    {
        UninstallCrashHandler();
    }
};
