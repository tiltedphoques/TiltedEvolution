
#include <BuildInfo.h>
#include <MinHook.h>
#include <TiltedCore/Initializer.hpp>

#include "Launcher.h"
#include "TargetConfig.h"

#include "loader/ExeLoader.h"
#include "loader/PathRerouting.h"

#include "Utils/Error.h"
#include "oobe/InstallCheckFlow.h"
#include "oobe/ViabilityChecks.h"
#include "steam/SteamLoader.h"

static LaunchContext* g_context = nullptr;

LaunchContext* GetLaunchContext()
{
    if (!g_context)
        __debugbreak();

    return g_context;
}

void Bootstrap()
{
    //LdrGetKnownDllSectionHandle = reinterpret_cast<decltype(LdrGetKnownDllSectionHandle)>(
    //    GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "LdrGetKnownDllSectionHandle"));

    auto LC = std::make_unique<LaunchContext>();
    g_context = LC.get();

    if (!oobe::TestPlatformViability(oobe::Policy::kRecommended))
    {
        Die("Your platform is not supported.");
        return;
    }

    if (!oobe::CheckInstall())
    {
        return;
    }

    // Bind path environment.
    loader::InstallPathRouting(LC->gamePath);
    steam::Load(LC->gamePath);

    {
        ExeLoader loader(CurrentTarget.loadLimit, GetProcAddress);
        if (!loader.Load(LC->exePath))
            return;

        LC->gameMain = loader.GetEntryPoint();
    }

    TiltedPhoques::Initializer::RunAll();
    LC->gameMain();
}

void RunClient()
{
    LoadLibraryA(
        R"(C:\Users\vince\Documents\Development\Tilted\TiltedEvolution\build\windows\x64\debug\SkyrimTogether.dll)");

    #if 0
    __debugbreak();
    LoadLibraryA(
        R"(C:\Users\vince\Documents\Development\Tilted\TiltedEvolution\build\windows\x64\debug\SkyrimTogether.dll)");
        __debugbreak();
        #endif
}
