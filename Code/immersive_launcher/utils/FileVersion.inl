#pragma once

#include <optional>
#include <Memory.hpp>
#include <Windows.h>

inline TiltedPhoques::String QueryFileVersion(const wchar_t* acpPath)
{
    // check if viable
    if (DWORD infoSize = GetFileVersionInfoSizeW(acpPath, nullptr))
    {
        auto buffer = TiltedPhoques::MakeUnique<uint8_t[]>(infoSize);
        if (GetFileVersionInfoW(acpPath, 0, infoSize, buffer.get()))
        {
            char* productVersion = nullptr;
            DWORD productVersionLen = 0;
            if (VerQueryValue(buffer.get(), "\\StringFileInfo\\040904B0\\ProductVersion", (void**)&productVersion,
                              (PUINT)&productVersionLen) &&
                productVersionLen && productVersion)
            {
                return productVersion;
            }

            if (VerQueryValueA(buffer.get(), "\\StringFileInfo\\040904B0\\FileVersion", (LPVOID*)&productVersion,
                               (PUINT)&productVersionLen) &&
                productVersionLen && productVersion && *productVersion)
            {
                return productVersion;
            }
        }
    }

    return {};
}
