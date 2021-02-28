
#include <FunctionHook.hpp>
#include <TiltedCore/Initializer.hpp>
#include "Launcher.h"

static Launcher* s_pLauncher = nullptr;
static std::once_flag s_initGuard;

void GetStartupInfoW_Hook(LPSTARTUPINFOW apInfo) noexcept
{
    std::call_once(s_initGuard, []() 
    { 
        s_pLauncher->LoadClient();
    });

    GetStartupInfoW(apInfo);
}

static decltype(&::_initterm) initterm_Orig = nullptr;

void initterm_Hook(_PVFV* apStart, _PVFV* apEnd)
{
    std::call_once(s_initGuard, []() 
    { 
        s_pLauncher->LoadClient();
    });

    initterm_Orig(apStart, apEnd);
}

void WINAPI RaiseException_Hook(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments,
                                const ULONG_PTR* lpArguments)
{
    if (dwExceptionCode == 0x406D1388 && !IsDebuggerPresent())
        return; // thread naming

    RaiseException(dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments);
}

DWORD WINAPI GetModuleFileNameW_Hook(HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
    if (!hModule)
    {
        auto& path = s_pLauncher->GetExePath();
        wcscpy_s(lpFilename, nSize, path.c_str());

        return static_cast<DWORD>(path.native().length());
    }

    return GetModuleFileNameW(hModule, lpFilename, nSize);
}

bool BootstrapGame(Launcher* apLauncher)
{
    s_pLauncher = apLauncher;
    auto appPath = TiltedPhoques::GetPath();
    auto& gamePath = apLauncher->GetGamePath();

    SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);
    AddDllDirectory(appPath.c_str());
    AddDllDirectory(gamePath.c_str());
    SetCurrentDirectoryW(gamePath.c_str());

    std::wstring pathBuf;
    pathBuf.resize(32768);
    GetEnvironmentVariableW(L"PATH", pathBuf.data(), static_cast<DWORD>(pathBuf.length()));

    // append bin & game directories
    std::wstring newPath = appPath.native() + L";" + gamePath.native() + L";" + pathBuf;
    SetEnvironmentVariableW(L"PATH", newPath.c_str());
    return true;
}

static TiltedPhoques::Initializer s_Init([] {
    TP_HOOK_IAT2("Kernel32.dll", "GetStartupInfoW", GetStartupInfoW_Hook);
    TP_HOOK_IAT2("Kernel32.dll", "GetModuleFileNameW", GetModuleFileNameW_Hook);
    TP_HOOK_IAT2("Kernel32.dll", "RaiseException", RaiseException_Hook);

    // Fallout4 uses a silly OLD CRT
    initterm_Orig =
        reinterpret_cast<decltype(&::_initterm)>(GetProcAddress(GetModuleHandleW(L"MSVCR110.dll"), "_initterm"));
    TP_HOOK_IAT2("MSVCR110.dll", "_initterm", initterm_Hook);
});
