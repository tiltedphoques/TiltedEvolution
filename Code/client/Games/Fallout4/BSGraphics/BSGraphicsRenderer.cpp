#include <TiltedOnlinePCH.h>

#include <BSGraphics/BSGraphicsRenderer.h>
#include <Renderer.h>
#include <Services/InputService.h>
#include <Services/OverlayService.h>
#include <World.h>
#include <Systems/RenderSystemD3D11.h>

#include <d3d11.h>

namespace BSGraphics
{
namespace
{
WNDPROC s_originalWndProc = nullptr;
RenderSystemD3D11* s_renderSystem = nullptr;
uint32_t s_windowWidth = 0;
uint32_t s_windowHeight = 0;

LRESULT CALLBACK HookWndProc(HWND aWindow, UINT aMessage, WPARAM aWParam, LPARAM aLParam)
{
    if (InputService::WndProc(aWindow, aMessage, aWParam, aLParam) != 0)
        return 0;
    // The game reads raw input here; keep it away from the game while the overlay has focus.
    if (aMessage == WM_INPUT && World::Get().ctx().at<OverlayService>().GetActive())
        return DefWindowProcW(aWindow, aMessage, aWParam, aLParam);
    return CallWindowProcW(s_originalWndProc, aWindow, aMessage, aWParam, aLParam);
}

TP_THIS_FUNCTION(TRendererEnd, void, void);
TRendererEnd* s_rendererEnd = nullptr;

void TP_MAKE_THISCALL(HookRendererEnd, void)
{
    auto* pData = BGSRenderer::Get();
    if (pData && pData->pD3dDevice && pData->pD3dContext && pData->pSwapChain && pData->windowHandle)
    {
        if (!s_renderSystem)
        {
            s_renderSystem = &World::Get().ctx().at<RenderSystemD3D11>();
            s_renderSystem->OnDeviceCreation(pData->pSwapChain, pData->pD3dDevice, pData->pD3dContext);
            s_originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(pData->windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&HookWndProc)));
            s_windowWidth = pData->windowWidth;
            s_windowHeight = pData->windowHeight;
        }
        else if (s_renderSystem->GetSwapChain() != pData->pSwapChain || s_windowWidth != pData->windowWidth || s_windowHeight != pData->windowHeight)
        {
            s_renderSystem->OnReset(pData->pSwapChain);
            s_windowWidth = pData->windowWidth;
            s_windowHeight = pData->windowHeight;
        }
        // The overlay's SpriteBatch needs a viewport; during loading the game may have none bound.
        UINT viewportCount = 1;
        D3D11_VIEWPORT viewport{};
        pData->pD3dContext->RSGetViewports(&viewportCount, &viewport);
        if (viewportCount == 0)
        {
            viewport = {0.f, 0.f, static_cast<float>(pData->windowWidth), static_cast<float>(pData->windowHeight), 0.f, 1.f};
            pData->pD3dContext->RSSetViewports(1, &viewport);
        }
        s_renderSystem->OnRender();
    }
    TiltedPhoques::ThisCall(s_rendererEnd, apThis);
}

// Fallout 4 quits at startup when a "Fallout4" window already exists. Hiding
// other instances lets several clients run on one machine.
using TFindWindowA = HWND(WINAPI)(LPCSTR, LPCSTR);
TFindWindowA* s_findWindowA = nullptr;

HWND WINAPI HookFindWindowA(LPCSTR apClassName, LPCSTR apWindowName)
{
    if (apClassName && !IS_INTRESOURCE(apClassName) && _stricmp(apClassName, "Fallout4") == 0)
        return nullptr;
    return s_findWindowA(apClassName, apWindowName);
}

TiltedPhoques::Initializer s_rendererHooks(
    []()
    {
        s_findWindowA = static_cast<TFindWindowA*>(TP_HOOK_IAT2("USER32.dll", "FindWindowA", HookFindWindowA));

        static VersionDbPtr<TRendererEnd> rendererEnd(2276834);
        s_rendererEnd = rendererEnd.Get();
        TP_HOOK(&s_rendererEnd, HookRendererEnd);
    });
}

RendererWindow* GetMainWindow()
{
    auto* pData = BGSRenderer::Get();
    return pData ? reinterpret_cast<RendererWindow*>(&pData->windowHandle) : nullptr;
}

bool RendererWindow::IsForeground()
{
    return GetForegroundWindow() == hWnd;
}
}
