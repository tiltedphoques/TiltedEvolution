#include <TiltedOnlinePCH.h>

#include <TiltedOnlineApp.h>

#include <DInputHook.hpp>
#include <WindowsHook.hpp>

#include <World.h>

#include <Systems/RenderSystemD3D11.h>

#include <Services/OverlayService.h>
#include <Services/ImguiService.h>
#include <Services/DiscordService.h>

#include <ScriptExtender.h>

using TiltedPhoques::Debug;

TiltedOnlineApp::TiltedOnlineApp()
{
}

TiltedOnlineApp::~TiltedOnlineApp() = default;

void* TiltedOnlineApp::GetMainAddress() const
{
    POINTER_SKYRIMSE(void, winMain, 36544);
    POINTER_FALLOUT4(void, winMain, 0x140D35930 - 0x140000000);

    return winMain.GetPtr();
}

bool TiltedOnlineApp::BeginMain()
{
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
    POINTER_SKYRIMSE(uint32_t, bUseFaceGenPreprocessedHeads, 378620);
    POINTER_FALLOUT4(uint32_t, bUseFaceGenPreprocessedHeads, 0x143733CE0 - 0x140000000);

    *bUseFaceGenPreprocessedHeads = 0;

    // Make sure the window stays active
    POINTER_SKYRIMSE(uint32_t, bAlwaysActive, 380768);
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

void TiltedOnlineApp::InstallHooks2()
{
    TiltedPhoques::Initializer::RunAll();

    TiltedPhoques::DInputHook::Install();
}

void TiltedOnlineApp::UninstallHooks()
{
    
}
