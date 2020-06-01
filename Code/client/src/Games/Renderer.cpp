
#if TP_PLATFORM_64

#include <ProcessMemory.hpp>

#include <Games/Skyrim/Renderer.h>
#include <Games/Fallout4/Renderer.h>

#include <Systems/RenderSystemD3D11.h>

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

static LRESULT wndProc_Stub(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //ImGui_ImplWin32_Init()
    return RealWndProc(hwnd, msg, wParam, lParam);
}

#if TP_SKYRIM64
static void HookPresent()
#else
static void HookPresent(LPCRITICAL_SECTION lock)
#endif
{
    // not so nice..
    TiltedPhoques::D3D11Hook::Get().OnPresent(BGSRenderer::Get()->pSwapChain);

#if TP_SKYRIM64
    return RealRenderPresent();
#else
    return RealRenderPresent(lock);
#endif
}

// TODO: handle lost devices

static bool HookCreateViewport(void *viewport, ViewportConfig *pConfig, WindowConfig *pWindowConfig, void *a4)
{
    // force windowed mode
    // TODO: make configurable
    // TURNS OUT ITS CONFIGURABLE via game ini:o
    //pWindowConfig->bBorderlessDisplay = false;
    //pWindowConfig->bFullScreenDisplay = false;

#if TP_SKYRIM64
    pConfig->name = "Skyrim Together";
#else
    pConfig->name = "Fallout Together";
#endif

    const auto result = RealCreateViewport(viewport, pConfig, pWindowConfig, a4);

    auto* pBGSRenderer = BGSRenderer::Get();
    TiltedPhoques::D3D11Hook::Get().OnCreate(pBGSRenderer->pSwapChain);

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
    TiltedPhoques::Write<DWORD>(0xD71FA9 + 1, WS_OVERLAPPEDWINDOW);

    // don't let the game steal the media keys in windowed mode
    TiltedPhoques::Write<BYTE>(0xC1A0B5 + 2, /*strip DISCL_EXCLUSIVE bits and append DISCL_NONEXCLUSIVE*/ 3);
#else
    TiltedPhoques::Write<DWORD>(0x1D17EE7 + 1, WS_OVERLAPPEDWINDOW);
#endif
});

#elif (TP_PLATFORM_32)

static TiltedPhoques::Initializer s_viewportHooks([]()
{
    //TODO: ask config if we should be started in windowed mode

    // fix style
    TiltedPhoques::Write<DWORD>(0x69D831 + 1, WS_OVERLAPPEDWINDOW);
    TiltedPhoques::Write<DWORD>(0x69D876 + 1, WS_OVERLAPPEDWINDOW);
});

#endif
