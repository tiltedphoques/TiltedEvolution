
#include "DllBlocklist.h"

namespace stubs
{
    // A list of modules blocked for a variety of reasons, but mainly
    // for causing crashes and incompatibility with ST
    const wchar_t* const kDllBlocklist[] = {
        L"EngineFixes.dll",    // Skyrim Engine Fixes, breaks our hooks
        L"crashhandler64.dll", // Stream crash handler, breaks heap
        L"fraps64.dll",        // Breaks tilted ui
    };

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
}
