
#include "Launcher.h"
#include <FunctionHook.hpp>
#include <TiltedCore/Meta.hpp>
#include <TiltedCore/Initializer.hpp>

static std::once_flag s_initGuard;
static uint16_t(WINAPI* Real_crtGetShowWindowMode)() = nullptr;

void TP_GetStartupInfoW(LPSTARTUPINFOW apInfo) noexcept
{
    std::call_once(s_initGuard, []() 
    { 
        GetLauncher()->LoadClient();
    });

    GetStartupInfoW(apInfo);
}

uint16_t TP_crtGetShowWindowMode()
{
    std::call_once(s_initGuard, []() 
    { 
         GetLauncher()->LoadClient(); 
    });

    return Real_crtGetShowWindowMode();
}

// this is more of a workaround, till we add SEH table support.
void WINAPI TP_RaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments,
                                const ULONG_PTR* lpArguments)
{
    if (dwExceptionCode == 0x406D1388 && !IsDebuggerPresent())
        return; // thread naming

    RaiseException(dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments);
}

static TiltedPhoques::Initializer s_Init([] {
    TP_HOOK_IAT2("Kernel32.dll", "GetStartupInfoW", TP_GetStartupInfoW);
    TP_HOOK_IAT2("Kernel32.dll", "RaiseException", TP_RaiseException);

    // Fallout4 is linked against an old CRT, so we need to apply our init
    // hooks differently
    HMODULE hMsvCrt = GetModuleHandleW(L"MSVCR110.dll");
    // shut up the compiler
    if (hMsvCrt)
    {
        // Fallout4 uses a silly OLD CRT
        Real_crtGetShowWindowMode = reinterpret_cast<decltype(Real_crtGetShowWindowMode)>(
            GetProcAddress(hMsvCrt, "__crtGetShowWindowMode"));
        TP_HOOK_IAT2("MSVCR110.dll", "__crtGetShowWindowMode", TP_crtGetShowWindowMode);
    }
});
