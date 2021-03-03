
#include <FunctionHook.hpp>
#include <TiltedCore/Initializer.hpp>
#include "Launcher.h"

static Launcher* s_pLauncher = nullptr;
static std::once_flag s_initGuard;

void TP_GetStartupInfoW(LPSTARTUPINFOW apInfo) noexcept
{
    std::call_once(s_initGuard, []() 
    { 
        s_pLauncher->LoadClient();
    });

    GetStartupInfoW(apInfo);
}

static uint16_t(WINAPI* Real_crtGetShowWindowMode)() = nullptr;

uint16_t TP_crtGetShowWindowMode()
{
    std::call_once(s_initGuard, []() 
    { 
         s_pLauncher->LoadClient(); 
    });

    return Real_crtGetShowWindowMode();
}


void WINAPI TP_RaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments,
                                const ULONG_PTR* lpArguments)
{
    if (dwExceptionCode == 0x406D1388 && !IsDebuggerPresent())
        return; // thread naming

    RaiseException(dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments);
}

// TODO: check for updates... etc

void BootstrapGame(Launcher* apLauncher)
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
}

static TiltedPhoques::Initializer s_Init([] {
    TP_HOOK_IAT2("Kernel32.dll", "GetStartupInfoW", TP_GetStartupInfoW);
    TP_HOOK_IAT2("Kernel32.dll", "RaiseException", TP_RaiseException);

    // Fallout4 uses a silly OLD CRT
    Real_crtGetShowWindowMode = reinterpret_cast<decltype(Real_crtGetShowWindowMode)>(
        GetProcAddress(GetModuleHandleW(L"MSVCR110.dll"), "__crtGetShowWindowMode"));
    TP_HOOK_IAT2("MSVCR110.dll", "__crtGetShowWindowMode", TP_crtGetShowWindowMode);
});
