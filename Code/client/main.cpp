
#include <TiltedOnlineApp.h>
#include <TiltedOnlinePCH.h>

std::unique_ptr<TiltedOnlineApp> g_appInstance{nullptr};

void RunTiltedInit(const std::filesystem::path& acGamePath, const String& aExeVersion,
                   std::shared_ptr<spdlog::logger> aLogPipe)
{
    spdlog::set_default_logger(aLogPipe);

    if (!VersionDb::Get().Load(acGamePath, aExeVersion))
    {
        MessageBoxA(
            NULL,
            "Could not load address library, install it: \"https://www.nexusmods.com/skyrimspecialedition/mods/32444\"",
            "Fatal error", 0);
        exit(-1);
    }

    // VersionDb::Get().Dump(R"(S:\Work\Tilted\Reverse\addresslib.txt)");

    g_appInstance = std::make_unique<TiltedOnlineApp>();

    TiltedOnlineApp::InstallHooks2();
    TP_HOOK_COMMIT;
}

void RunTiltedApp()
{
    g_appInstance->BeginMain();
}
