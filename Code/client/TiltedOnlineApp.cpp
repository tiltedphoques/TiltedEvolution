#include <TiltedOnlinePCH.h>

#include <TiltedOnlineApp.h>

#include <DInputHook.hpp>
#include <WindowsHook.hpp>

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
    auto logPath = TiltedPhoques::GetPath() / "logs";

    std::error_code ec;
    create_directory(logPath, ec);

    auto rotatingLogger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath / "tp_client.log", 1048576 * 5, 3);
    //rotatingLogger->set_level(spdlog::level::debug);
    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console->set_pattern("%^[%H:%M:%S] [%l]%$ %v");

    auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{ console, rotatingLogger });
    set_default_logger(logger);
}

TiltedOnlineApp::~TiltedOnlineApp() = default;

void* TiltedOnlineApp::GetMainAddress() const
{
    POINTER_SKYRIMSE(void, winMain, 36544);
    POINTER_FALLOUT4(void, winMain, 668529);

    return winMain.GetPtr();
}

bool TiltedOnlineApp::BeginMain()
{
    World::Create();
    World::Get().ctx().at<DiscordService>().Init();
    World::Get().ctx().emplace<RenderSystemD3D11>(World::Get().ctx().at<OverlayService>(), World::Get().ctx().at<ImguiService>());

    LoadScriptExender();
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
    POINTER_SKYRIMSE(uint32_t, bUseFaceGenPreprocessedHeads, 378620);
    POINTER_FALLOUT4(uint32_t, bUseFaceGenPreprocessedHeads, 196397);

    *bUseFaceGenPreprocessedHeads = 0;

    // Make sure the window stays active
    POINTER_SKYRIMSE(uint32_t, bAlwaysActive, 380768);
    POINTER_FALLOUT4(uint32_t, bAlwaysActive, 1420078);

    *bAlwaysActive = 1;

    World::Get().Update();
}

bool TiltedOnlineApp::Attach()
{
    TiltedPhoques::Debug::OnAttach();

    //TiltedPhoques::Nop(0x1405D3FA1, 6);
    return true;
}

bool TiltedOnlineApp::Detach()
{
    TiltedPhoques::Debug::OnDetach();
    return true;
}

void TiltedOnlineApp::InstallHooks2()
{
    TiltedPhoques::Initializer::RunAll();

    TiltedPhoques::DInputHook::Install();
}

void TiltedOnlineApp::UninstallHooks()
{
    
}
