#include <Games/System.h>

using TFindWindowA = HWND(WINAPI*)(_In_opt_ LPCSTR lpClassName, _In_opt_ LPCSTR lpWindowName);
TFindWindowA RealFindWindowA = nullptr;

static HWND WINAPI HookFindWindowA(_In_opt_ LPCSTR lpClassName, _In_opt_ LPCSTR lpWindowName)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    return nullptr;
}

static TiltedPhoques::Initializer s_systemHooks([]()
    {
        TP_HOOK_IAT(FindWindowA, "user32.dll");
    });
