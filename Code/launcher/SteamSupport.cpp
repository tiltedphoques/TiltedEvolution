
#include <cstdio>

#include "SteamSupport.h"
#include "Utils/Registry.h"
#include <TiltedCore/Filesystem.hpp>
#include <MinHook.h>
#include <intrin.h>

namespace fs = std::filesystem;

const wchar_t kSteamDllName[] = L"steamclient64.dll";
const wchar_t kSteamCrashHandlerDllName[] = L"crashhandler64.dll";

void SteamLoad(TitleId aTid, const fs::path& aGamePath)
{
    auto appId = std::to_wstring(ToSteamAppId(aTid));
    SetEnvironmentVariableW(L"SteamAppId", appId.c_str());

    auto path = aGamePath / "steam_appid.txt";

    FILE* f = nullptr;
    _wfopen_s(&f, path.c_str(), L"w");
    if (f)
    {
        fprintf(f, "%d", ToSteamAppId(aTid));
        fclose(f);
    }

    fs::path steamPath = Registry::ReadString<wchar_t>(HKEY_CURRENT_USER, 
        LR"(SOFTWARE\Valve\Steam)", L"SteamPath");

    // wait what
    if (!fs::exists(steamPath))
        return;

    AddDllDirectory(steamPath.c_str());

    fs::path clientPath = steamPath / kSteamDllName;
    fs::path crashHandlerPath = steamPath / kSteamCrashHandlerDllName;

    LoadLibraryW(clientPath.c_str());
    LoadLibraryW(crashHandlerPath.c_str());
}
