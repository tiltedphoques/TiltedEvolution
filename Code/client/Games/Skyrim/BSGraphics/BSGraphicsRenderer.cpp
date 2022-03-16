
#include "Services/InputService.h"
#include "Systems/RenderSystemD3D11.h"

#include "World.h"

#include "BSGraphics/BSGraphicsRenderer.h"

extern UINT g_SuggestedDeviceByLauncher;

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
    aOSData->pClassName = "Skyrim Together EVO Test";
    RealWndProc = aOSData->pWndProc;
    aOSData->pWndProc = Hook_WndProc;

    Renderer_Init(self, aOSData, aFBData, aOut);

    g_sRs = &World::Get().ctx<RenderSystemD3D11>();
    // This how the game does it too
    g_RenderWindow = &self->Data.RenderWindowA[0];

    g_sRs->OnDeviceCreation(self->Data.RenderWindowA[0].pSwapChain);
}

struct Scaleform_Render_Viewport
{
    int BufferWidth;
    int BufferHeight;
    int Left;
    int Top;
    int Width;
    int Height;
    int ScissorLeft;
    int ScissorTop;
    int ScissorWidth;
    int ScissorHeight;
    unsigned int Flags;
};

struct Scaleform_GFx_Viewport : Scaleform_Render_Viewport
{
    float Scale;
    float AspectRatio;
};

template <typename T> struct Scaleform_Render_Rect
{
    T a, b, c, d;
};

enum Scaleform_GFx_Movie_ScaleModeType : __int32
{
    SM_NoScale = 0x0,
    SM_ShowAll = 0x1,
    SM_ExactFit = 0x2,
    SM_NoBorder = 0x3,
};

int (*Scaleform_GFx_MovieImpl_SetViewport)(void*, const Scaleform_GFx_Viewport*);
void (*sub_140F67210)(void*, const Scaleform_Render_Rect<float>*);
void (*Scaleform_GFx_MovieImpl_SetViewScaleMode)(void*, Scaleform_GFx_Movie_ScaleModeType);

float (*CalculateSafeZoneInterface_X)();
float (*CalculateSafeZoneInterface_Y)();

int Hook_Scaleform_GFx_MovieImpl_SetViewport(void* a1, const Scaleform_GFx_Viewport* a2)
{
    return Scaleform_GFx_MovieImpl_SetViewport(a1, a2);
}

static bool onceISay = false;

static void ResizeMovieViewport(void* apMovie)
{
#if 1
    // the games function comes with ultrawide support.
    float safeZoneX = CalculateSafeZoneInterface_X();
    float safeZoneY = CalculateSafeZoneInterface_Y();

    auto* pState = (BSGraphics::State*)0x1430C6D90;

    float v22 = (float)pState->uiBackBufferWidth - safeZoneX;
    float v23 = (float)pState->uiBackBufferHeight - safeZoneY;
    float v24 = 1.0 / (float)pState->uiBackBufferWidth;
    float v61 = v24 * v22;
    float v25 = 1.0 / (float)pState->uiBackBufferHeight;

    Scaleform_GFx_Viewport newVp{};
    newVp.Width = pState->uiBackBufferWidth;
    newVp.Height = pState->uiBackBufferHeight;
    newVp.Flags = 1065353216;
    newVp.Scale = 1.f;
    newVp.AspectRatio = 1.f;
    void* movieView = *(void**)0x142FE9150;
    if (movieView && onceISay)
    {
        Scaleform_GFx_MovieImpl_SetViewScaleMode(movieView, Scaleform_GFx_Movie_ScaleModeType::SM_ShowAll);

        const Scaleform_Render_Rect<float> bounds{.a = v24, .b = v25, .c = safeZoneX, .d = safeZoneY

        };
        sub_140F67210(movieView, &bounds);

        Scaleform_GFx_MovieImpl_SetViewport(movieView, &newVp);
    }
#endif
}

void (*Renderer_ResetWindow)(BSGraphics::Renderer*, uint32_t);

void Hook_Renderer_ResetWindow(BSGraphics::Renderer* self, uint32_t auiIndex)
{
    Renderer_ResetWindow(self, auiIndex);

    g_sRs->OnReset(self->Data.RenderWindowA[0].pSwapChain);


    ResizeMovieViewport(nullptr);
#if 0
    for (void* ptr : g_knownMovies)
    {
        if (!IsBadReadPtr(ptr, 8))
        {
            Scaleform_GFx_MovieImpl_SetViewport(ptr, &newVp);
        }
    }
#endif
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

    Scaleform_GFx_MovieImpl_SetViewport =
        static_cast<decltype(Scaleform_GFx_MovieImpl_SetViewport)>((void*)(0x140F71A30));
    sub_140F67210 = static_cast<decltype(sub_140F67210)>((void*)(0x140F67210));

    CalculateSafeZoneInterface_X = static_cast<decltype(CalculateSafeZoneInterface_X)>((void*)(0x140F11310));
    CalculateSafeZoneInterface_Y = static_cast<decltype(CalculateSafeZoneInterface_Y)>((void*)(0x140F11360));
    Scaleform_GFx_MovieImpl_SetViewScaleMode =
        static_cast<decltype(Scaleform_GFx_MovieImpl_SetViewScaleMode)>((void*)(0x140F71A10));

    TP_HOOK_IMMEDIATE(&Scaleform_GFx_MovieImpl_SetViewport, &Hook_Scaleform_GFx_MovieImpl_SetViewport);
});
} // namespace BSGraphics
