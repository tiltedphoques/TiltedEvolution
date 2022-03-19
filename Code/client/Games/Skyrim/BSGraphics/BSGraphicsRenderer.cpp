
#include "Services/InputService.h"
#include "Systems/RenderSystemD3D11.h"

#include "World.h"

#include "BSGraphics/BSGraphicsRenderer.h"
#include "Interface/UI.h"

extern HICON g_SharedWindowIcon;

namespace BSGraphics
{
static RenderSystemD3D11* g_sRs = nullptr;
static WNDPROC RealWndProc = nullptr;
static RendererWindow* g_RenderWindow = nullptr;

RendererWindow* GetMainWindow()
{
    return g_RenderWindow;
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
    // NOTE(Force): This forces the game to use the UI suggested by the launcher.
    // aOSData->uiAdapter = g_SuggestedDeviceByLauncher;

    // we feed this a shared icon as the resource directory of our former launcher data is already overwritten with the
    // game.
    aOSData->hIcon = g_SharedWindowIcon;

    aOSData->pClassName = "Skyrim Together EVO Test";
    RealWndProc = aOSData->pWndProc;
    aOSData->pWndProc = Hook_WndProc;

    Renderer_Init(self, aOSData, aFBData, aOut);

    g_sRs = &World::Get().ctx<RenderSystemD3D11>();
    // This how the game does it too
    g_RenderWindow = &self->Data.RenderWindowA[0];

    g_sRs->OnDeviceCreation(self->Data.RenderWindowA[0].pSwapChain);
}

void (*Renderer_ResetWindow)(BSGraphics::Renderer*, uint32_t);

void Hook_Renderer_ResetWindow(BSGraphics::Renderer* self, uint32_t auiIndex)
{
    Renderer_ResetWindow(self, auiIndex);

    g_sRs->OnReset(self->Data.RenderWindowA[0].pSwapChain);

    UI::Get()->ResizeMovies();
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
    const VersionDbPtr<uint8_t> initWindowLoc(77226);
    // patch dwStyle in BSGraphics::InitWindows
    TiltedPhoques::Put(initWindowLoc.Get() + 0x174 + 1, WS_OVERLAPPEDWINDOW);

    const VersionDbPtr<uint8_t> windowLoc(68781);
    // TODO: move me to input patches.
    // don't let the game steal the media keys in windowed mode
    TiltedPhoques::Put(windowLoc.Get() + 0x55 + 2, /*strip DISCL_EXCLUSIVE bits and append DISCL_NONEXCLUSIVE*/ 3);

    const VersionDbPtr<uint8_t> timerLoc(77246);
    const VersionDbPtr<uint8_t> renderInit(77226);

    TiltedPhoques::SwapCall(timerLoc.Get() + 9, StopTimer, &Hook_StopTimer);

    Renderer_Init = static_cast<decltype(Renderer_Init)>(renderInit.GetPtr());

    // Once we find a proper way to locate it for different versions, go back to swapcall
    // TiltedPhoques::SwapCall(mem::pointer(initLoc.GetPtr()) + 0xD1A, Renderer_Init, &Hook_Renderer_Init);
    TP_HOOK_IMMEDIATE(&Renderer_Init, &Hook_Renderer_Init);

    TiltedPhoques::SwapCall(0x140DA59B5, Renderer_ResetWindow, &Hook_Renderer_ResetWindow);
    TiltedPhoques::SwapCall(0x140DA59EF, Renderer_ResetWindow, &Hook_Renderer_ResetWindow);

    // TiltedPhoques::Put(0x1430C6DE1, true);
    // TiltedPhoques::Put(0x140F11094, 0xC3);

    // TP_HOOK_IMMEDIATE(&Scaleform_GFx_MovieImpl_SetViewport, &Hook_Scaleform_GFx_MovieImpl_SetViewport);
});
} // namespace BSGraphics
