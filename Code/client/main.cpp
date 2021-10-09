
#include <TiltedOnlinePCH.h>
#include <TiltedOnlineApp.h>

static std::unique_ptr<TiltedOnlineApp> g_appInstance{nullptr};

void RunTiltedApp()
{
    g_appInstance = std::make_unique<TiltedOnlineApp>();

    TP_HOOK_COMMIT;

    g_appInstance->BeginMain();
}
