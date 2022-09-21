#include "CrashHandler.h"
#include <DbgHelp.h>
#include <Windows.h>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <strsafe.h>

#include <base/dialogues/win/TaskDialog.h>

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

extern HICON g_SharedWindowIcon;

LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    if (pExceptionInfo->ExceptionRecord->ExceptionCode == 0xC0000005)
    {
        std::ostringstream oss;
        oss << "crash_" << SerializeTimePoint(std::chrono::system_clock::now(), "UTC_%Y-%m-%d_%H-%M-%S") << ".dmp";

        char dumpPath[MAX_PATH];

        GetModuleFileNameA(NULL, dumpPath, sizeof(dumpPath));
        std::filesystem::path modulePath(dumpPath);
        auto subPath = modulePath.parent_path();

        subPath /= oss.str();

        // TODO: use this if instead before merging
//#if (!IS_MASTER)
#if 1
        auto dumpLocation = fmt::format(L"The crash dump file will have the following name: {}", subPath.filename().c_str());

        Base::TaskDialog dia(g_SharedWindowIcon, L"Crash", L"A crash has occurred.",
                             L"Do you want to generate a crash dump?",
                             dumpLocation.c_str());

        dia.AppendButton(0xBEED, L"Yes");
        dia.AppendButton(0xBEEF, L"No");
        const int result = dia.Show();

        if (result != 0xBEED)
            return EXCEPTION_EXECUTE_HANDLER;
#endif

        CrashHandler::RemovePreviousDump(subPath);

        MINIDUMP_EXCEPTION_INFORMATION M;
        M.ThreadId = GetCurrentThreadId();
        M.ExceptionPointers = pExceptionInfo;
        M.ClientPointers = 0;

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
