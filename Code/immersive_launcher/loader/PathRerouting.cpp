// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "PathRerouting.h"
#include <TiltedCore/Filesystem.hpp>
#include <Windows.h>

namespace loader
{
void InstallPathRouting(const std::filesystem::path& gamePath)
{
    // will install improper path routing here...
    auto appPath = TiltedPhoques::GetPath();

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
} // namespace loader
