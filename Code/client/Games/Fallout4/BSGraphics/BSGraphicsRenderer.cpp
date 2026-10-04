#include <TiltedOnlinePCH.h>

#include <BSGraphics/BSGraphicsRenderer.h>
#include <Renderer.h>
#include <Services/InputService.h>
#include <Systems/RenderSystemD3D11.h>

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
        s_renderSystem->OnRender();
    }
    TiltedPhoques::ThisCall(s_rendererEnd, apThis);
}

TiltedPhoques::Initializer s_rendererHooks(
    []()
    {
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
