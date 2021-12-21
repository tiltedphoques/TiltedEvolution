
#include <BuildInfo.h>
#include <MinHook.h>

#include <TiltedCore/Initializer.hpp>
#include <TiltedReverse/Code/reverse/include/Debug.hpp>

#include "TargetConfig.h"
#include "launcher.h"

#include "loader/ExeLoader.h"
#include "loader/PathRerouting.h"

#include "Utils/Error.h"
#include "Utils/FileVersion.inl"

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

static void PreloadSystemDlls()
{
    auto systemDlls = {L"\\dinput8.dll", // < Skyrim early init hook
                       L"\\dsound.dll",  // breaks DSound init in game code
                                         // X360CE v3 is buggy with COM hooks
                       L"\\xinput9_1_0.dll", L"\\xinput1_1.dll", L"\\xinput1_2.dll", L"\\xinput1_3.dll",
                       L"\\xinput1_4.dll", L"\\version.dll"};

    for (auto dll : systemDlls)
    {
        LoadLibraryW(dll);
    }
}

LaunchContext* GetLaunchContext()
{
    if (!g_context)
        __debugbreak();

    return g_context;
}

// Everything is nothing, life is worth living, just look to the stars
#define DIE_NOW(err)                                                                                                   \
    {                                                                                                                  \
        Die(err);                                                                                                      \
        return false;                                                                                                  \
    }

int StartUp(int argc, char** argv)
{
    PreloadSystemDlls();

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
            DIE_NOW(ec);
    }

    if (!oobe::SelectInstall(askSelect))
        DIE_NOW("Failed to select game install.");

    // Bind path environment.
    loader::InstallPathRouting(LC->gamePath);
    steam::Load(LC->gamePath);

    if (!LoadProgram(*LC))
        return 3;

    InstallStartHook();
    // Initialize all hooks before calling game init
    // TiltedPhoques::Initializer::RunAll();
    RunTiltedInit();

    // This shouldn't return until the game is killed
    LC->gameMain();
    return 0;
}

bool LoadProgram(LaunchContext& LC)
{
    auto content = TiltedPhoques::LoadFile(LC.exePath);
    if (content.empty())
        DIE_NOW("Failed to mount game executable");

    auto versionString = QueryFileVersion(LC.exePath.c_str());
    if (versionString.empty())
        DIE_NOW("Failed to query game version");

    if (content.size() != CurrentTarget.exeDiskSz || versionString != CurrentTarget.gameVerson)
    {
        auto err =
            fmt::format("Unsupported game version.\n(Executable doesn't match)\nExpected version {}\nGot version {}",
                        CurrentTarget.gameVerson, versionString.c_str());

        DIE_NOW(err.c_str());
    }

    ExeLoader loader(CurrentTarget.exeLoadSz);
    if (!loader.Load(reinterpret_cast<uint8_t*>(content.data())))
        DIE_NOW("Fatal error while mapping executable");

    LC.gameMain = loader.GetEntryPoint();
    return true;
}

void InitClient()
{
    // Jump into client code.
    RunTiltedApp();
}
} // namespace launcher
