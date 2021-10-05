
#include "SteamLoader.h"
#include "GameConfig.h"

#include "Utils/Registry.h"
#include <cstdio>

namespace steam
{
constexpr wchar_t kSteamDllName[] = L"steamclient64.dll";
constexpr wchar_t kSteamCrashHandlerDllName[] = L"crashhandler64.dll";

void Load(const fs::path& aGameDir)
{
    auto appId = std::to_wstring(kGame.steamAppId);
    SetEnvironmentVariableW(L"SteamAppId", appId.c_str());

    auto path = aGameDir / "steam_appid.txt";

    FILE* f = nullptr;
    _wfopen_s(&f, path.c_str(), L"w");
    if (f)
    {
        fprintf(f, "%d", kGame.steamAppId);
        fclose(f);
    }

    fs::path steamPath = Registry::ReadString<wchar_t>(HKEY_CURRENT_USER, LR"(SOFTWARE\Valve\Steam)", L"SteamPath");

    // wait what
    if (!fs::exists(steamPath))
        return;

    AddDllDirectory(steamPath.c_str());

    fs::path clientPath = steamPath / kSteamDllName;
    fs::path crashHandlerPath = steamPath / kSteamCrashHandlerDllName;

    LoadLibraryW(clientPath.c_str());
    LoadLibraryW(crashHandlerPath.c_str());
}
} // namespace steam
