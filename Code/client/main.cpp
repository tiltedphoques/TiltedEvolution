
#include <TiltedOnlineApp.h>
#include <TiltedOnlinePCH.h>
#include <ScriptExtender.h>

#include <Commctrl.h>
#include <Windows.h>

#include <base/dialogues/win/TaskDialog.h>

// 0 - Steam, 1 - GOG. One supported patch per game for now.
inline constexpr std::string_view kSupportedGameVersions[2] = {SUPPORTED_GAME_VERSION, SUPPORTED_GAME_VERSION};

std::unique_ptr<TiltedOnlineApp> g_appInstance{nullptr};

extern HICON g_SharedWindowIcon;

static void ShowAddressLibraryError(const wchar_t* apGamePath)
{
#if defined(TP_FALLOUT4)
    auto errorDetail = fmt::format(L"Looking for it here: {}\\Data\\F4SE\\Plugins", apGamePath);

    Base::TaskDialog dia(g_SharedWindowIcon, L"Error", L"Failed to load Fallout 4 Address Library", L"Make sure to use \"All in one\" (version-1-11-240-0.bin)", errorDetail.c_str());

    dia.AppendButton(0xBEEF, L"Visit the F4SE address library page on nexusmods.com");
    const int result = dia.Show();
    if (result == 0xBEEF)
    {
        ShellExecuteW(nullptr, L"open", LR"(https://www.nexusmods.com/fallout4/mods/47327)", nullptr, nullptr, SW_SHOWNORMAL);
    }
#else
    auto errorDetail = fmt::format(L"Looking for it here: {}\\Data\\SKSE\\Plugins", apGamePath);

    Base::TaskDialog dia(g_SharedWindowIcon, L"Error", L"Failed to load Skyrim Address Library", L"Make sure to use \"All in one\"", errorDetail.c_str());

    dia.AppendButton(0xBEED, L"Visit troubleshooting page on wiki.tiltedphoques.com");
    dia.AppendButton(0xBEEF, L"Visit Address Library modpage on nexusmods.com");
    const int result = dia.Show();
    if (result == 0xBEEF)
    {
        ShellExecuteW(nullptr, L"open", LR"(https://www.nexusmods.com/skyrimspecialedition/mods/32444?tab=files)", nullptr, nullptr, SW_SHOWNORMAL);
    }
    else if (result == 0xBEED)
    {
        ShellExecuteW(nullptr, L"open", LR"(https://wiki.tiltedphoques.com/tilted-online/guides/troubleshooting/address-library-error)", nullptr, nullptr, SW_SHOWNORMAL);
    }
#endif

    exit(4);
}

static void ShowIncompatibleVersionError(const char* apDetectedGameVersion, const wchar_t* apGamePath)
{
    constexpr wchar_t kModPageUrl[] =
#if defined(TP_FALLOUT4)
        LR"(https://www.nexusmods.com/fallout4/mods/69993)"; // TODO(Fallout4): proper mod page
#else
        LR"(https://www.nexusmods.com/skyrimspecialedition/mods/69993?tab=files)";
#endif
    const auto [steamVer, gogVer] = kSupportedGameVersions;

    std::string supportedVersions = steamVer != gogVer ? fmt::format("{} (or {} if GOG)", steamVer, gogVer) : std::string{steamVer};
    std::string message = fmt::format("{} {} requires {} {}, but your installed version is {}\n\nUpdate or downgrade to match, then relaunch", PRODUCT_NAME_A, BUILD_COMMIT + 1, SHORT_NAME_A, supportedVersions, apDetectedGameVersion);
    std::wstring wideMessage(message.begin(), message.end());

    const auto optionalDetails = fmt::format(L"Installed here: {}", apGamePath);

    Base::TaskDialog dia(g_SharedWindowIcon, L"Error", L"Incompatible game version", wideMessage.c_str(), optionalDetails.c_str());
    dia.AppendButton(0xBEEF, L"Visit the mod page on nexusmods.com");

    if (dia.Show() == 0xBEEF)
    {
        ShellExecuteW(nullptr, L"open", kModPageUrl, nullptr, nullptr, SW_SHOWNORMAL);
    }
    exit(4);
}

void RunTiltedInit(const std::filesystem::path& acGamePath, const String& aExeVersion)
{
    if (!VersionDb::Get().Load(acGamePath, aExeVersion))
    {
        ShowAddressLibraryError(acGamePath.c_str());
    }

    auto [steamVer, gogVer] = kSupportedGameVersions;
    if (aExeVersion != steamVer && aExeVersion != gogVer)
    {
        ShowIncompatibleVersionError(aExeVersion.c_str(), acGamePath.c_str());
    }

    g_appInstance = std::make_unique<TiltedOnlineApp>();

    TiltedOnlineApp::InstallHooks2();
    TP_HOOK_COMMIT;

    LoadScriptExtender();
}

void RunTiltedApp()
{
    g_appInstance->BeginMain();
}
