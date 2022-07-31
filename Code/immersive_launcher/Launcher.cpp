
#include <TiltedReverse/Code/reverse/include/Debug.hpp>
#include <BranchInfo.h>

#include "TargetConfig.h"
#include "launcher.h"

#include "loader/ExeLoader.h"
#include "loader/PathRerouting.h"

#include "Utils/Error.h"
#include "Utils/FileVersion.inl"

#include "oobe/PathSelection.h"
#include "oobe/SupportChecks.h"
#include "steam/SteamLoader.h"

#include "base/dialogues/win/TaskDialog.h"

// These symbols are defined within the client code skyrimtogetherclient
extern void InstallStartHook();
extern void RunTiltedApp();
extern void RunTiltedInit(const std::filesystem::path& acGamePath, const TiltedPhoques::String& aExeVersion);

// Defined in EarlyLoad.dll
bool __declspec(dllimport) EarlyInstallSucceeded();

HICON g_SharedWindowIcon = nullptr;

namespace launcher
{
static LaunchContext* g_context = nullptr;

LaunchContext* GetLaunchContext()
{
    #if 0
    if (!g_context)
        __debugbreak();
    #endif
    return g_context;
}

// Everything is nothing, life is worth living, just look to the stars
#define DIE_NOW(err)                                                                                                   \
    {                                                                                                                  \
        Die(err);                                                                                                      \
        return false;                                                                                                  \
    }


void SetMaxstdio()
{
    const auto handle = GetModuleHandleW(L"API-MS-WIN-CRT-STDIO-L1-1-0.DLL");
    if (!handle)
        return;

    const auto setmaxstdioFunc = reinterpret_cast<decltype(&_setmaxstdio)>(GetProcAddress(handle, "_setmaxstdio"));

    if (!setmaxstdioFunc)
        return;

    setmaxstdioFunc(8192);
}

int StartUp(int argc, char** argv)
{
    bool askSelect = (GetAsyncKeyState(VK_SPACE) & 0x8000);
    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(argv[i], "-r") == 0)
            askSelect = true;
    }

    // TODO(Force): Make some InitSharedResources func.
    g_SharedWindowIcon = LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(102));

#if (!IS_MASTER)
    TiltedPhoques::Debug::CreateConsole();
#endif

    SetMaxstdio();

    if (!EarlyInstallSucceeded())
        DIE_NOW(L"Early load install failed. Tell Force about this.");

    auto LC = std::make_unique<LaunchContext>();
    g_context = LC.get();

    {
        const wchar_t* ec = nullptr;
        const auto status = oobe::ReportModCompatabilityStatus();
        switch (status)
        {
        case oobe::CompatabilityStatus::kDX11Unsupported:
            ec = L"Device does not support DirectX 11";
            break;
        case oobe::CompatabilityStatus::kOldOS:
            ec = L"Operating system unsupported. Please upgrade to Windows 8.1 or greater";
            break;
        }

        if (ec)
            DIE_NOW(ec);
    }

    if (!oobe::SelectInstall(askSelect))
        DIE_NOW(L"Failed to select game install.");

    // Bind path environment.
    loader::InstallPathRouting(LC->gamePath);
    steam::Load(LC->gamePath);

    if (!LoadProgram(*LC))
        return 3;

    InstallStartHook();
    // Initialize all hooks before calling game init
    // TiltedPhoques::Initializer::RunAll();
    RunTiltedInit(LC->gamePath, LC->Version);

    // This shouldn't return until the game is killed
    LC->gameMain();
    return 0;
}

bool LoadProgram(LaunchContext& LC)
{
    auto content = TiltedPhoques::LoadFile(LC.exePath);
    if (content.empty())
        DIE_NOW(L"Failed to mount game executable");

    LC.Version = QueryFileVersion(LC.exePath.c_str());
    if (LC.Version.empty())
        DIE_NOW(L"Failed to query game version");

    ExeLoader loader(CurrentTarget.exeLoadSz);
    if (!loader.Load(reinterpret_cast<uint8_t*>(content.data())))
        DIE_NOW(L"Fatal error while mapping executable");

    LC.gameMain = loader.GetEntryPoint();
    return true;
}

void InitClient()
{
    // Jump into client code.
    RunTiltedApp();
}
} // namespace launcher

// CreateProcess in suspended mode.
// Inject usvfs_64.dll -> invoke InitHooks
// (https://github.com/ModOrganizer2/usvfs/blob/f8051c179dee114b7e06c5dab2482977c285d611/src/usvfs_dll/usvfs.cpp#L352)
// Resume proc


// InjectDLLRemoteThread ->SkipInit

