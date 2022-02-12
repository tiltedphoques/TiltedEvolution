
#include <TiltedOnlinePCH.h>
#include <TiltedOnlineApp.h>

std::unique_ptr<TiltedOnlineApp> g_appInstance{nullptr};

void RunTiltedInit(const std::filesystem::path& acGamePath, const String& aExeVersion)
{
    if(!VersionDb::Get().Load(acGamePath, aExeVersion))
    {
        MessageBoxA(
            NULL,
            "Could not load address library, install it: \"https://www.nexusmods.com/skyrimspecialedition/mods/32444\"",
            "Fatal error", 0);
        exit(-1);
    }

    g_appInstance = std::make_unique<TiltedOnlineApp>();

    TiltedOnlineApp::InstallHooks2();
    TP_HOOK_COMMIT;
}

void RunTiltedApp()
{
    g_appInstance->BeginMain();
}
