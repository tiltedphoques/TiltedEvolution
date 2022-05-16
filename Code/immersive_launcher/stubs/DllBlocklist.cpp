
#include "DllBlocklist.h"

namespace stubs
{
// clang-format off

    // A list of modules blocked for a variety of reasons, but mainly
    // for causing crashes and incompatibility with ST
    const wchar_t* const kDllBlocklist[] = {
#if defined(TARGET_ST)
        L"EngineFixes.dll",          // Skyrim Engine Fixes, breaks our hooks
        L"SkyrimSoulsRE.dll",        // Our mod implements this with special handling
#elif defined(TARGET_FT)
        L"FallSouls.dll",            // Our mod implements this with special handling
#endif
        L"crashhandler64.dll",       // Stream crash handler, breaks heap
        L"fraps64.dll",              // Breaks tilted ui
        L"SpecialK64.dll",           // breaks rendering
        L"ReShade64_SpecialK64.dll", // same reason
        L"NvCamera64.dll",           // broken af nvidia stuff, blacklisted for now, needs fix later
       // L"atiuxp64.dll",
       // L"aticfx64.dll"
    };

// clang-format on

bool IsDllBlocked(std::wstring_view dllName)
{
    for (const wchar_t* dllEntry : kDllBlocklist)
    {
        if (std::wcscmp(dllName.data(), dllEntry) == 0)
        {
            return true;
        }
    }

    return false;
}
} // namespace stubs
