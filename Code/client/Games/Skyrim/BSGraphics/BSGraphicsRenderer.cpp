
#include "Services/InputService.h"
#include "Systems/RenderSystemD3D11.h"

#include "World.h"

#include "BSGraphics/BSGraphicsRenderer.h"
#include "BSRandom/BSRandom.h"

// shared resource by launcher
extern HICON g_SharedWindowIcon;

namespace BSGraphics
{
namespace
{

static RenderSystemD3D11* g_sRs = nullptr;
static WNDPROC RealWndProc = nullptr;
static RendererWindow* g_RenderWindow = nullptr;

static constexpr char kTogetherWindowName[]{"Skyrim Together"};

} // namespace
RendererWindow* GetMainWindow()
{
    return g_RenderWindow;
}

bool RendererWindow::IsForeground()
{
    return GetForegroundWindow() == hWnd;
}

void (*Renderer_Init)(Renderer*, BSGraphics::RendererInitOSData*, const BSGraphics::ApplicationWindowProperties*,
                      BSGraphics::RendererInitReturn*) = nullptr;

// WNDPROC seems to be part of the renderer
LRESULT CALLBACK Hook_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (InputService::WndProc(hwnd, uMsg, wParam, lParam) != 0)
        return 0;

    return RealWndProc(hwnd, uMsg, wParam, lParam);
}

void Hook_Renderer_Init(Renderer* self, BSGraphics::RendererInitOSData* aOSData,
                        const BSGraphics::ApplicationWindowProperties* aFBData, BSGraphics::RendererInitReturn* aOut)
{
    // we feed this a shared icon as the resource directory of our former launcher data is already overwritten with the
    // game.
    aOSData->hIcon = g_SharedWindowIcon;
    // Append our window name.
    aOSData->pClassName = kTogetherWindowName;

    RealWndProc = aOSData->pWndProc;
    aOSData->pWndProc = Hook_WndProc;

    Renderer_Init(self, aOSData, aFBData, aOut);

    g_sRs = &World::Get().ctx().at<RenderSystemD3D11>();
    // This how the game does it too
    g_RenderWindow = &self->Data.RenderWindowA[0];

    g_sRs->OnDeviceCreation(self->Data.RenderWindowA[0].pSwapChain);
}

void (*StopTimer)(int) = nullptr;

// Insert us at the End
void Hook_StopTimer(int type)
{
    if (g_sRs)
        g_sRs->OnRender();

    StopTimer(type);
}

static TiltedPhoques::Initializer s_viewportHooks([]() {
    const VersionDbPtr<void> initWindowLoc(77226);
    // patch dwStyle in BSGraphics::InitWindows
    TiltedPhoques::Put(mem::pointer(initWindowLoc.GetPtr()) + 0x174 + 1, WS_OVERLAPPEDWINDOW);

    const VersionDbPtr<void> windowLoc(68781);
    // TODO: move me to input patches.
    // don't let the game steal the media keys in windowed mode
    TiltedPhoques::Put(mem::pointer(windowLoc.GetPtr()) + 0x55 + 2,
                       /*strip DISCL_EXCLUSIVE bits and append DISCL_NONEXCLUSIVE*/ 3);

    const VersionDbPtr<void> timerLoc(77246);
    const VersionDbPtr<void> renderInit(77226);

    TiltedPhoques::SwapCall(mem::pointer(timerLoc.GetPtr()) + 9, StopTimer, &Hook_StopTimer);

    Renderer_Init = static_cast<decltype(Renderer_Init)>(renderInit.GetPtr());

    // Once we find a proper way to locate it for different versions, go back to swapcall
    // TiltedPhoques::SwapCall(mem::pointer(initLoc.GetPtr()) + 0xD1A, Renderer_Init, &Hook_Renderer_Init);
    TP_HOOK_IMMEDIATE(&Renderer_Init, &Hook_Renderer_Init);
});
} // namespace BSGraphics
