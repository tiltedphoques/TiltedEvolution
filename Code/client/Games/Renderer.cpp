#include <TiltedOnlinePCH.h>


#include <Renderer.h>

#include <Systems/RenderSystemD3D11.h>
#include <Services/DiscordService.h>

#include <World.h>
#include <D3D11Hook.hpp>

using TCreateViewport = bool(void*, ViewportConfig*, WindowConfig*, void*);

#if TP_SKYRIM64
using TRenderPresent = void();
#else
using TRenderPresent = void(LPCRITICAL_SECTION);
#endif

static WNDPROC RealWndProc = nullptr;
static TCreateViewport* RealCreateViewport = nullptr;
static TRenderPresent* RealRenderPresent = nullptr;

BGSRenderer* BGSRenderer::Get()
{
    POINTER_SKYRIMSE(BGSRenderer*, s_instance, 411347);
    POINTER_FALLOUT4(BGSRenderer*, s_instance, 1235450);

    return *(s_instance.Get());
}

ID3D11Device* BGSRenderer::GetDevice()
{
    POINTER_SKYRIMSE(ID3D11Device*, s_device, 411348);
    POINTER_FALLOUT4(ID3D11Device*, s_device, 454122);

    return *(s_device.Get());
}

#if TP_SKYRIM64
static void HookPresent()
#else
static void HookPresent(LPCRITICAL_SECTION lock)
#endif
{
    // TODO (Force): refactor this ..
    TiltedPhoques::D3D11Hook::Get().OnPresent(BGSRenderer::Get()->pSwapChain);

#if TP_SKYRIM64
    return RealRenderPresent();
#else
    return RealRenderPresent(lock);
#endif
}

// TODO (Force): handle lost devices

static bool HookCreateViewport(void *viewport, ViewportConfig *pConfig, WindowConfig *pWindowConfig, void *a4)
{
#if TP_SKYRIM64
    pConfig->name = "Skyrim Together | " BUILD_BRANCH "@" BUILD_COMMIT;
#else
    pConfig->name = "Fallout Together | " BUILD_BRANCH "@" BUILD_COMMIT;
#endif

#if 0
    pWindowConfig->bBorderlessDisplay = false;
    pWindowConfig->bFullScreenDisplay = false;
#endif

    const auto result = RealCreateViewport(viewport, pConfig, pWindowConfig, a4);

    auto* pSwapchain = BGSRenderer::Get()->pSwapChain;
    TiltedPhoques::D3D11Hook::Get().OnCreate(pSwapchain);

    return result;
}

static TiltedPhoques::Initializer s_viewportHooks([]()
{
#ifndef TP_SKYRIM64

    POINTER_SKYRIMSE(TCreateViewport, s_realCreateViewport, 77226);
    POINTER_FALLOUT4(TCreateViewport, s_realCreateViewport, 564406);

    RealCreateViewport = s_realCreateViewport.Get();
    TP_HOOK(&RealCreateViewport, HookCreateViewport);

    POINTER_SKYRIMSE(TRenderPresent, s_realRenderPresent, 77246);
    POINTER_FALLOUT4(TRenderPresent, s_realRenderPresent, 700870);

    RealRenderPresent = s_realRenderPresent.Get();
    TP_HOOK(&RealRenderPresent, HookPresent);


    TiltedPhoques::Put(0x140000000 +
         (0x1D17EE7 + 1), WS_OVERLAPPEDWINDOW);
#endif
});
