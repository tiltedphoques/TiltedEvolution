
#include "launcher.h"
#include <FunctionHook.hpp>
#include <mutex>
#include <TiltedCore/Initializer.hpp>

static std::once_flag s_initGuard;
static uint16_t(WINAPI* Real_crtGetShowWindowMode)() = nullptr;
static int(WINAPI* Real_ismbbled)(uint32_t) = nullptr;

void TP_GetStartupInfoW(LPSTARTUPINFOW apInfo) noexcept
{
    std::call_once(s_initGuard, []() { launcher::InitClient(); });
    GetStartupInfoW(apInfo);
}

int TP_ismbblead(uint32_t c)
{
    std::call_once(s_initGuard, []() { launcher::InitClient(); });
    return Real_ismbbled(c);
}

// this is more of a workaround, till we add SEH table support.
void WINAPI TP_RaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments,
                              const ULONG_PTR* lpArguments)
{
    if (dwExceptionCode == 0x406D1388 && !IsDebuggerPresent())
        return; // thread naming

    RaiseException(dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments);
}

void InstallStartHook()
{
#if defined(TARGET_FT)
    // Fallout4 is linked against an old silly CRT, so we need to apply our init
    // hooks differently
    if (auto hCrt = GetModuleHandleW(L"MSVCR110.dll") /*Shut up the compiler*/)
    {
        Real_ismbbled = reinterpret_cast<decltype(Real_ismbbled)>(GetProcAddress(hCrt, "_ismbblead"));
        TP_HOOK_IAT2("MSVCR110.dll", "_ismbblead", TP_ismbblead);
    }
#elif defined(TARGET_ST)
    TP_HOOK_IAT2("Kernel32.dll", "GetStartupInfoW", TP_GetStartupInfoW);
    TP_HOOK_IAT2("Kernel32.dll", "RaiseException", TP_RaiseException);
#else
#error Fix?
#endif
};
