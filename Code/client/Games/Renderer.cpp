#include <TiltedOnlinePCH.h>


#include <MemoryVP.hpp>

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
    POINTER_SKYRIMSE(BGSRenderer*, s_instance, 0x143025F00 - 0x140000000);
    POINTER_FALLOUT4(BGSRenderer*, s_instance, 0x14609BF80 - 0x140000000);

    return *(s_instance.Get());
}

ID3D11Device* BGSRenderer::GetDevice()
{
    POINTER_SKYRIMSE(ID3D11Device*, s_device, 0x143025F08 - 0x140000000);
    POINTER_FALLOUT4(ID3D11Device*, s_device, 0x1461E0918 - 0x140000000);

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

    const wchar_t *pCmdl = GetCommandLineW();
    if (std::wcsstr(pCmdl, L"+force_windowed"))
    {
        pWindowConfig->bBorderlessDisplay = false;
        pWindowConfig->bFullScreenDisplay = false;
    }

    const auto result = RealCreateViewport(viewport, pConfig, pWindowConfig, a4);

    auto* pSwapchain = BGSRenderer::Get()->pSwapChain;
    TiltedPhoques::D3D11Hook::Get().OnCreate(pSwapchain);

    return result;
}

static TiltedPhoques::Initializer s_viewportHooks([]()
{
    POINTER_SKYRIMSE(TCreateViewport, s_realCreateViewport, 0x140D68DD0 - 0x140000000);
    POINTER_FALLOUT4(TCreateViewport, s_realCreateViewport, 0x141D09DA0 - 0x140000000);

    RealCreateViewport = s_realCreateViewport.Get();
    TP_HOOK(&RealCreateViewport, HookCreateViewport);

    POINTER_SKYRIMSE(TRenderPresent, s_realRenderPresent, 0x140D6A2B0 - 0x140000000);
    POINTER_FALLOUT4(TRenderPresent, s_realRenderPresent, 0x141D0B670 - 0x140000000);

    RealRenderPresent = s_realRenderPresent.Get();
    TP_HOOK(&RealRenderPresent, HookPresent);

#if TP_SKYRIM64
    // change window mode style to have a close button
    TiltedPhoques::vp::ScopedContext(0x140000000 + (0xD71FA9 + 1), WS_OVERLAPPEDWINDOW);

    // don't let the game steal the media keys in windowed mode
    TiltedPhoques::vp::ScopedContext(0x140000000 + (0xC1A0B5 + 2),
                                     /*strip DISCL_EXCLUSIVE bits and append DISCL_NONEXCLUSIVE*/ 3);
#else
    TiltedPhoques::vp::ScopedContext(0x140000000 +
         (0x1D17EE7 + 1), WS_OVERLAPPEDWINDOW);
#endif
});
