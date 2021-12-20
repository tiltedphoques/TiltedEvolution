
#include <BuildInfo.h>
#include <MinHook.h>

#include <TiltedCore/Initializer.hpp>
#include <TiltedReverse/Code/reverse/include/Debug.hpp>

#include "TargetConfig.h"
#include "launcher.h"

#include "loader/ExeLoader.h"
#include "loader/PathRerouting.h"

#include "Utils/Error.h"
#include "oobe/PathSelection.h"
#include "oobe/SupportChecks.h"
#include "steam/SteamLoader.h"

// These symbols are defined within the client code
extern void InstallStartHook();
extern void RunTiltedApp();
extern void RunTiltedInit();

namespace launcher
{
static LaunchContext* g_context = nullptr;

LaunchContext* GetLaunchContext()
{
    if (!g_context)
        __debugbreak();

    return g_context;
}

int StartUp(int argc, char** argv)
{
    // VK_E
    bool askSelect = (GetAsyncKeyState(0x45) & 0x8000);
    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(argv[i], "-r") == 0)
            askSelect = true;
    }

#if (!IS_MASTER)
    TiltedPhoques::Debug::CreateConsole();
#endif

    auto r = GetLastError();

    auto LC = std::make_unique<LaunchContext>();
    g_context = LC.get();

    {
        const char* ec = nullptr;
        const auto status = oobe::ReportModCompatabilityStatus();
        switch (status)
        {
        case oobe::CompatabilityStatus::kDX11Unsupported:
            ec = "Device does not support DirectX 11";
            break;
        case oobe::CompatabilityStatus::kOldOS:
            ec = "Operating system unsupported. Please upgrade to Windows 8.1 or greater";
            break;
        }

        if (ec)
        {
            Die(ec);
            return false;
        }
    }

    if (!oobe::SelectInstall(askSelect))
    {
        Die("Failed to select game install.");
        return false;
    }

    // Bind path environment.
    loader::InstallPathRouting(LC->gamePath);
    steam::Load(LC->gamePath);

    {
        ExeLoader loader(CurrentTarget.exeSize, GetProcAddress);
        if (!loader.Load(LC->exePath))
            return 3;

        LC->gameMain = loader.GetEntryPoint();
    }

    InstallStartHook();
    // Initialize all hooks before calling game init
    // TiltedPhoques::Initializer::RunAll();
    RunTiltedInit();

    // This shouldn't return until the game is killed
    LC->gameMain();
    return 0;
}

void InitClient()
{
    // Jump into client code.
    RunTiltedApp();
}
} // namespace launcher
