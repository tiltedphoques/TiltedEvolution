#include <TiltedOnlinePCH.h>

#include <Renderer.h>

#include <Systems/RenderSystemD3D11.h>
#include <Services/DiscordService.h>

#include <World.h>
#include <D3D11Hook.hpp>

using TCreateViewport = bool(void*, ViewportConfig*, WindowConfig*, void*);

using TRenderPresent = void();

static WNDPROC RealWndProc = nullptr;
static TCreateViewport* RealCreateViewport = nullptr;
static TRenderPresent* RealRenderPresent = nullptr;

BGSRenderer* BGSRenderer::Get()
{
    POINTER_SKYRIMSE(BGSRenderer*, s_instance, 411347);

    return *(s_instance.Get());
}

ID3D11Device* BGSRenderer::GetDevice()
{
    POINTER_SKYRIMSE(ID3D11Device*, s_device, 411348);

    return *(s_device.Get());
}

static void HookPresent()
{
    // TODO (Force): refactor this ..
    TiltedPhoques::D3D11Hook::Get().OnPresent(BGSRenderer::Get()->pSwapChain);

    return RealRenderPresent();
}

// TODO (Force): handle lost devices

static bool HookCreateViewport(void* viewport, ViewportConfig* pConfig, WindowConfig* pWindowConfig, void* a4)
{
    pConfig->name = "Skyrim Together | " BUILD_BRANCH "@" BUILD_COMMIT;

#if 0
    pWindowConfig->bBorderlessDisplay = false;
    pWindowConfig->bFullScreenDisplay = false;
#endif

    const auto result = RealCreateViewport(viewport, pConfig, pWindowConfig, a4);

    auto* pSwapchain = BGSRenderer::Get()->pSwapChain;
    TiltedPhoques::D3D11Hook::Get().OnCreate(pSwapchain);

    return result;
}

static TiltedPhoques::Initializer s_viewportHooks(
    []()
    {
    });
