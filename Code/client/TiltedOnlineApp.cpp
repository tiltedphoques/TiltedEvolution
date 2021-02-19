#include <TiltedOnlineApp.h>

#include <TiltedHooks/Code/hooks/include/DInputHook.hpp>
#include <TiltedHooks/Code/hooks/include/WindowsHook.hpp>

#include <World.h>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <Systems/RenderSystemD3D11.h>

#include <Services/OverlayService.h>
#include <Services/ImguiService.h>
#include <Services/DiscordService.h>

#include <ScriptExtender.h>

using TiltedPhoques::Debug;

TiltedOnlineApp::TiltedOnlineApp()
{
    Debug::CreateConsole();

    std::error_code ec;
    std::filesystem::create_directory("logs", ec);

    auto rotatingLogger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/tp_client.log", 1048576 * 5, 3);
    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console->set_pattern("%^[%H:%M:%S] [%l]%$ %v");

    auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{ console, rotatingLogger });
    spdlog::set_default_logger(logger);
}

TiltedOnlineApp::~TiltedOnlineApp() = default;

void* TiltedOnlineApp::GetMainAddress() const
{
    POINTER_SKYRIMSE(void, winMain, 0x1405ACBD0 - 0x140000000);
    POINTER_FALLOUT4(void, winMain, 0x140D35930 - 0x140000000);

    return winMain.GetPtr();
}

bool TiltedOnlineApp::BeginMain()
{
    InstallHooks();

    World::Create();
    World::Get().ctx<DiscordService>().Init();
    World::Get().set<RenderSystemD3D11>(World::Get().ctx<OverlayService>(), World::Get().ctx<ImguiService>());

    InjectScriptExtenderDll();
    return true;
}

bool TiltedOnlineApp::EndMain()
{
    UninstallHooks();

    return true;
}

void TiltedOnlineApp::Update()
{
    // Every frame make sure we won't use preprocessed facegen
    POINTER_SKYRIMSE(uint32_t, bUseFaceGenPreprocessedHeads, 0x141DEA030 - 0x140000000);
    POINTER_FALLOUT4(uint32_t, bUseFaceGenPreprocessedHeads, 0x143733CE0 - 0x140000000);

    *bUseFaceGenPreprocessedHeads = 0;

    // Make sure the window stays active
    POINTER_SKYRIMSE(uint32_t, bAlwaysActive, 0x141DEED10 - 0x140000000);
    POINTER_FALLOUT4(uint32_t, bAlwaysActive, 0x14378E618 - 0x140000000);

    *bAlwaysActive = 1;

    World::Get().Update();
}

bool TiltedOnlineApp::Attach()
{
    TiltedPhoques::Debug::OnAttach();
    return true;
}

bool TiltedOnlineApp::Detach()
{
    TiltedPhoques::Debug::OnDetach();
    return true;
}

void TiltedOnlineApp::InstallHooks()
{
    TiltedPhoques::Initializer::RunAll();

    TiltedPhoques::DInputHook::Install();
    TiltedPhoques::WindowsHook::Install();
}

void TiltedOnlineApp::UninstallHooks()
{
    
}
