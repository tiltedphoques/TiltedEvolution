
#include <BuildInfo.h>
#include <TiltedCore/Initializer.hpp>

#include "Launcher.h"
#include "loader/ExeLoader.h"

#include "oobe/InstallCheckFlow.h"
#include "oobe/ViabilityChecks.h"
#include "steam/SteamLoader.h"

#include "Utils/Error.h"

// How much memory may the loader use during start at the most
constexpr uintptr_t kGameLoadLimit = 0x140000000 + 0x70000000;

static LaunchContext* g_context = nullptr;

LaunchContext* GetLaunchContext()
{
    if (!g_context)
        __debugbreak();

    return g_context;
}

static void InitEnvironment()
{
    auto& gamePath = g_context->gamePath;
    auto appPath = TiltedPhoques::GetPath();

    SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);
    AddDllDirectory(appPath.c_str());
    AddDllDirectory(gamePath.c_str());
    SetCurrentDirectoryW(gamePath.c_str());

    std::wstring pathBuf;
    pathBuf.resize(32768);
    GetEnvironmentVariableW(L"PATH", pathBuf.data(), static_cast<DWORD>(pathBuf.length()));

    // append bin & game directories
    std::wstring newPath = appPath.native() + L";" + gamePath.native() + L";" + pathBuf;
    SetEnvironmentVariableW(L"PATH", newPath.c_str());
}

void Bootstrap()
{
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
    InitEnvironment();
    steam::Load(g_context->gamePath);

    {
        ExeLoader loader(kGameLoadLimit, GetProcAddress);
        if (!loader.Load(g_context->exePath))
            return;

        g_context->gameMain = loader.GetEntryPoint();
    }

    TiltedPhoques::Initializer::RunAll();
    g_context->gameMain();
}

void RunClient()
{

}
