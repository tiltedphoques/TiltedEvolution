
#include <TiltedReverse/Code/reverse/include/Debug.hpp>

#define SPDLOG_WCHAR_FILENAMES
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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
extern void RunTiltedInit(const std::filesystem::path&, const TiltedPhoques::String&, std::shared_ptr<spdlog::logger>);

HICON g_SharedWindowIcon = nullptr;

namespace launcher
{
namespace
{
void CreateLogger()
{
    auto logPath = TiltedPhoques::GetPath() / "logs";

    std::error_code ec;
    create_directory(logPath, ec);

    auto rotatingLogger =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath / "tp_client.log", 1048576 * 5, 3);
    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console->set_pattern("%^[%H:%M:%S] [%l]%$ %v");

    auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{console, rotatingLogger});
    set_default_logger(logger);
}

LaunchContext* g_context = nullptr;
} // namespace

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
    CreateLogger();

    // TODO(Force): Make some InitSharedResources func.
    g_SharedWindowIcon = LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(102));

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

    spdlog::info("Initializing Skyrim Together: Expanded and Enhanced Edition");
    RunTiltedInit(LC->gamePath, LC->Version, spdlog::default_logger());

    spdlog::info("Starting game");
    // This shouldn't return until the game is killed
    LC->gameMain();
    return 0;
}

bool LoadProgram(LaunchContext& LC)
{
    auto content = TiltedPhoques::LoadFile(LC.exePath);
    if (content.empty())
        DIE_NOW("Failed to mount game executable");

    LC.Version = QueryFileVersion(LC.exePath.c_str());
    if (LC.Version.empty())
        DIE_NOW("Failed to query game version");

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
