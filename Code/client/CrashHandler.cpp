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
    if (pExceptionInfo->ExceptionRecord->ExceptionCode == 0xC0000005)
    {
        MINIDUMP_EXCEPTION_INFORMATION M;
        char dumpPath[MAX_PATH];

        M.ThreadId = GetCurrentThreadId();
        M.ExceptionPointers = pExceptionInfo;
        M.ClientPointers = 0;

        std::ostringstream oss;
        oss << "crash_" << SerializeTimePoint(std::chrono::system_clock::now(), "UTC_%Y-%m-%d_%H-%M-%S") << ".dmp";

        GetModuleFileNameA(NULL, dumpPath, sizeof(dumpPath));
        std::filesystem::path modulePath(dumpPath);
        auto subPath = modulePath.parent_path();

        CrashHandler::RemovePreviousDump(subPath);

        subPath /= oss.str();

        auto hDumpFile =
            CreateFileA(subPath.string().c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

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

        CloseHandle(hDumpFile);

        return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

CrashHandler::CrashHandler()
{
    AddVectoredExceptionHandler(1, &VectoredExceptionHandler);
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
