
#include <TiltedOnlineApp.h>
#include <TiltedOnlinePCH.h>
#include <ScriptExtender.h>
#include <immersive_launcher/TargetConfig.h>
#if defined(TP_FALLOUT4)
#include <Offsets.h>
#endif

#include <Commctrl.h>
#include <Windows.h>

#include <base/dialogues/win/TaskDialog.h>

#if defined(TP_FALLOUT4)
// 0 - Steam, 1 - GOG. One supported patch per game for now.
inline constexpr std::string_view kSupportedGameVersions[2] = {SUPPORTED_GAME_VERSION, SUPPORTED_GAME_VERSION};
#else
// 1 - Steam, 2 - GOG
#ifdef TP_SKYRIM_1_6_1170
inline constexpr std::string_view kSupportedGameVersions[2] = {"1.6.1170.0", "1.6.1179.0"};
#else
inline constexpr std::string_view kSupportedGameVersions[2] = {"1.7.104.0", "1.7.104.0"};
#endif
#endif

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
    const auto [steamVer, gogVer] = kSupportedGameVersions;

    std::string supportedVersions = steamVer != gogVer ? fmt::format("{} (or {} if GOG)", steamVer, gogVer) : std::string{steamVer};
    const std::string commit = BUILD_COMMIT + 1;
    const std::string detectedVersion = apDetectedGameVersion;
    const auto wideMessage = fmt::format(L"{} {} requires {} {}, but your installed version is {}\n\nUpdate or downgrade to match, then relaunch", PRODUCT_NAME, std::wstring(commit.begin(), commit.end()), SHORT_NAME, std::wstring(supportedVersions.begin(), supportedVersions.end()), std::wstring(detectedVersion.begin(), detectedVersion.end()));

    const auto optionalDetails = fmt::format(L"Installed here: {}", apGamePath);

    Base::TaskDialog dia(g_SharedWindowIcon, L"Error", L"Incompatible game version", wideMessage.c_str(), optionalDetails.c_str());
#if defined(TP_FALLOUT4)
    // Fallout Together has no mod page yet.
    dia.Show();
#else
    dia.AppendButton(0xBEEF, L"Visit the mod page on nexusmods.com");

    if (dia.Show() == 0xBEEF)
    {
        ShellExecuteW(nullptr, L"open", LR"(https://www.nexusmods.com/skyrimspecialedition/mods/69993?tab=files)", nullptr, nullptr, SW_SHOWNORMAL);
    }
#endif
    exit(4);
}

void RunTiltedInit(const std::filesystem::path& acGamePath, const String& aExeVersion)
{
    auto [steamVer, gogVer] = kSupportedGameVersions;
    if (aExeVersion != steamVer && aExeVersion != gogVer)
    {
        ShowIncompatibleVersionError(aExeVersion.c_str(), acGamePath.c_str());
    }

#if defined(TP_FALLOUT4)
    if (!fo4::ValidateImage(acGamePath / "Fallout4.exe"))
    {
        Base::TaskDialog dia(g_SharedWindowIcon, L"Error", L"Unsupported Fallout 4 executable",
                             L"This build requires the Steam Fallout 4 1.11.240 executable.", nullptr);
        dia.Show();
        exit(4);
    }
#endif

    if (!VersionDb::Get().Load(acGamePath, aExeVersion))
    {
        ShowAddressLibraryError(acGamePath.c_str());
    }

    g_appInstance = std::make_unique<TiltedOnlineApp>();

    TiltedOnlineApp::InstallHooks2();
    TP_HOOK_COMMIT;

    int major, minor, revision, patch;
    VersionDb::Get().GetLoadedVersion(major, minor, revision, patch);
    LoadScriptExtender(acGamePath, {static_cast<uint32_t>(major), static_cast<uint32_t>(minor), static_cast<uint32_t>(revision), static_cast<uint32_t>(patch)});
}

void RunTiltedApp()
{
    g_appInstance->BeginMain();
}
