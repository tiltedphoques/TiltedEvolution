
#include <TiltedOnlineApp.h>
#include <TiltedOnlinePCH.h>

#include <Commctrl.h>
#include <Windows.h>

#include <base/dialogues/win/TaskDialog.h>

std::unique_ptr<TiltedOnlineApp> g_appInstance{nullptr};

extern HICON g_SharedWindowIcon;

static void ShowAddressLibraryError(const wchar_t* apGamePath)
{
#if TP_SKYRIM64
    auto errorDetail = fmt::format(L"Looking for it here: {}\\Data\\SKSE\\Plugins", apGamePath);

    Base::TaskDialog dia(g_SharedWindowIcon, L"Error", L"Failed to load Skyrim Address Library",
                         L"Make sure to use the All in one Anniversary Edition", errorDetail.c_str());
#elif TP_FALLOUT4
    auto errorDetail = fmt::format(L"Looking for it here: {}\\Data\\F4SE\\Plugins", apGamePath);

    Base::TaskDialog dia(g_SharedWindowIcon, L"Error", L"Failed to load Fallout 4 Address Library",
                         L"", errorDetail.c_str());
#endif

    dia.AppendButton(0xBEEF, L"Visit Address Library modpage on nexusmods.com");
    const int result = dia.Show();
    if (result == 0xBEEF)
    {
#if TP_SKYRIM64
        ShellExecuteW(nullptr, L"open", LR"(https://www.nexusmods.com/skyrimspecialedition/mods/32444?tab=files)",
                      nullptr, nullptr, SW_SHOWNORMAL);
#elif TP_FALLOUT4
        ShellExecuteW(nullptr, L"open", LR"(https://www.nexusmods.com/fallout4/mods/47327)",
                      nullptr, nullptr, SW_SHOWNORMAL);
#endif
    }

    exit(4);
}

void RunTiltedInit(const std::filesystem::path& acGamePath, const String& aExeVersion)
{
#if TP_SKYRIM64
    if (!VersionDb::Get().Load(acGamePath, aExeVersion))
#else
    if (!VersionDb::Get().LoadFallout4(acGamePath))
#endif
    {
        ShowAddressLibraryError(acGamePath.c_str());
    }

    g_appInstance = std::make_unique<TiltedOnlineApp>();

    TiltedOnlineApp::InstallHooks2();
    TP_HOOK_COMMIT;
}

void RunTiltedApp()
{
    g_appInstance->BeginMain();
}
