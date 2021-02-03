#pragma once

// Utility to import the 3rd-party Script Extender made by Ian Patterson et al.
// Required for non-Creation Kit mods such as SkyUI

#include <filesystem>

struct FileVersion
{
    static constexpr uint8_t scVersionSize = 4;
    DWORD versions[scVersionSize];
};

int GetFileVersion(const std::filesystem::path& aFilePath, FileVersion& aVersion);
void InjectScriptExtenderDll();
