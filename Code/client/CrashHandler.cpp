#include "CrashHandler.h"
#include <DbgHelp.h>
#include <Windows.h>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <strsafe.h>

using time_point = std::chrono::system_clock::time_point;

std::string SerializeTimePoint(const time_point& time, const std::string& format)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(time);
    std::tm tm = *std::gmtime(&tt); // GMT (UTC)
    // std::tm tm = *std::localtime(&tt); //Locale time-zone, usually UTC by default.
    std::stringstream ss;
    ss << std::put_time(&tm, format.c_str());
    return ss.str();
}

LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    static int alreadyCrashed = 0;
    auto retval = EXCEPTION_CONTINUE_SEARCH;

    // Serialize 
    static std::mutex singleThreaded;;
    const std::lock_guard lock{singleThreaded};

    // Check for severe, not continuable and not software-originated exception
    if (pExceptionInfo->ExceptionRecord->ExceptionCode >= 0x80000000 &&
        (pExceptionInfo->ExceptionRecord->ExceptionFlags & EXCEPTION_SOFTWARE_ORIGINATE) == 0 && 
        alreadyCrashed++ == 0)
    {
        spdlog::critical (__FUNCTION__ ": crash occurred!"); 
        
        auto present = IsDebuggerPresent() ? "already" : "not";
        spdlog::error(__FUNCTION__ ": debugger is {} present at critical exception time, code{:x}, address{}, flags {:x} ",
                      present,
                      pExceptionInfo->ExceptionRecord->ExceptionCode,
                      pExceptionInfo->ExceptionRecord->ExceptionAddress,
                      pExceptionInfo->ExceptionRecord->ExceptionFlags);

#if (IS_MASTER)
        volatile static bool bMiniDump = false;
#else
        volatile static bool bMiniDump = true;
#endif
        if (bMiniDump)
        {
            HANDLE hDumpFile = NULL;
            try
            {
                MINIDUMP_EXCEPTION_INFORMATION M;
                char dumpPath[MAX_PATH];

                M.ThreadId = GetCurrentThreadId();
                M.ExceptionPointers = pExceptionInfo;
                M.ClientPointers = 0;

                std::ostringstream oss;
                oss << "crash_" << SerializeTimePoint(std::chrono::system_clock::now(), "UTC_%Y-%m-%d_%H-%M-%S")
                    << ".dmp";

                GetModuleFileNameA(NULL, dumpPath, sizeof(dumpPath));
                std::filesystem::path modulePath(dumpPath);
                auto subPath = modulePath.parent_path();

                CrashHandler::RemovePreviousDump(subPath);

                subPath /= oss.str();

                hDumpFile = CreateFileA(subPath.string().c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                        FILE_ATTRIBUTE_NORMAL, NULL);

                // baseline settings from https://stackoverflow.com/a/63123214/5273909
                auto dumpSettings = MiniDumpWithDataSegs | MiniDumpWithProcessThreadData | MiniDumpWithHandleData |
                                    MiniDumpWithThreadInfo |
                                    /*
                                    //MiniDumpWithPrivateReadWriteMemory | // this one gens bad dump
                                    MiniDumpWithUnloadedModules |
                                    MiniDumpWithFullMemoryInfo |
                                    MiniDumpWithTokenInformation |
                                    MiniDumpWithPrivateWriteCopyMemory |
                                    */
                                    0;

                MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, (MINIDUMP_TYPE)dumpSettings,
                                  (pExceptionInfo) ? &M : NULL, NULL, NULL);
            }
            catch (...) // Mini-dump is best effort only.
            {
            }

            if (!hDumpFile)
                spdlog::critical(__FUNCTION__ ": coredump may have failed.");
            else
            {
                CloseHandle(hDumpFile);
                spdlog::critical(__FUNCTION__ ": coredump created -> flush logs.");
            }
        }

        spdlog::default_logger()->flush();

        // Something in STR breaks top-level unhandled exception filters.
        // The Win API for them is pretty clunky (non-atomic, not chainable), 
        // but they can do some important things. If someone actually set one
        // they probably meant it; make sure it actually runs.
        // This will make more CrashLogger mods work with STR.

        // Get the current unhandled exception filter. If it has changed
        // from when STR started up, invoke it here.
        LPTOP_LEVEL_EXCEPTION_FILTER pCurrentUnhandledExceptionFilter = SetUnhandledExceptionFilter(CrashHandler::GetOriginalUnhandledExceptionFilter());
        SetUnhandledExceptionFilter(pCurrentUnhandledExceptionFilter);
        if (pCurrentUnhandledExceptionFilter != CrashHandler::GetOriginalUnhandledExceptionFilter())
        {
            spdlog::critical(__FUNCTION__ ": UnhandledExceptionFilter() workaround triggered.");

            singleThreaded.unlock();        // Might reenter, but is safe at this point.
            if ((*pCurrentUnhandledExceptionFilter)(pExceptionInfo) == EXCEPTION_CONTINUE_EXECUTION)
                retval = EXCEPTION_CONTINUE_EXECUTION;
            singleThreaded.lock();
        }
    }
    return retval;
}

LPTOP_LEVEL_EXCEPTION_FILTER CrashHandler::m_pUnhandled;
CrashHandler::CrashHandler()
{
    // Record the original (or as close as we can get) top-level unhandled exception handler.
    // We grab this so we can see if it is changed, presumably by a mod or even graphics drivers.
    // Something in STR breaks unhandled exception handling, so we'll fake it if necessary.
    // This is the only way to get the current setting, but the race is small.
    m_pUnhandled = SetUnhandledExceptionFilter(NULL);
    SetUnhandledExceptionFilter(m_pUnhandled);

    m_handler = AddVectoredExceptionHandler(1, &VectoredExceptionHandler);
}

CrashHandler::~CrashHandler()
{
}

void CrashHandler::RemovePreviousDump(std::filesystem::path path)
{
    for (auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.path().string().find("crash") != std::string::npos)
        {
            DeleteFileA(entry.path().string().c_str());
        }
    }
}
