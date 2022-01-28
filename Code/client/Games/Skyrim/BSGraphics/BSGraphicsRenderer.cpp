
#include "Systems/RenderSystemD3D11.h"
#include "Services/InputService.h"

#include "World.h"

#include "BSGraphics/BSGraphicsRenderer.h"

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
    // Append our window name.
    aOSData->pClassName = "ST Anniversary | " BUILD_BRANCH "@" BUILD_COMMIT;

    RealWndProc = aOSData->pWndProc;
    aOSData->pWndProc = Hook_WndProc;

    Renderer_Init(self, aOSData, aFBData, aOut);

    g_sRs = &World::Get().ctx<RenderSystemD3D11>();
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
    // patch dwStyle in BSGraphics::InitWindows
    TiltedPhoques::Put(0x140DA38E4 + 1, WS_OVERLAPPEDWINDOW);

    // TODO: move me to input patches.
    // don't let the game steal the media keys in windowed mode
    TiltedPhoques::Put(0x140C40235 + 2, /*strip DISCL_EXCLUSIVE bits and append DISCL_NONEXCLUSIVE*/ 3);

    TiltedPhoques::SwapCall(0x140DA5B09, StopTimer, &Hook_StopTimer);
    TiltedPhoques::SwapCall(0x1405D4C2A, Renderer_Init, &Hook_Renderer_Init);
});
} // namespace BSGraphics
