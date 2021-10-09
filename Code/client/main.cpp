
#include <TiltedOnlinePCH.h>
#include <TiltedOnlineApp.h>

std::unique_ptr<TiltedOnlineApp> g_appInstance{nullptr};

void RunTiltedInit()
{
    g_appInstance = std::make_unique<TiltedOnlineApp>();

    TP_HOOK_COMMIT;
}

void RunTiltedApp()
{
    g_appInstance->BeginMain();
}
