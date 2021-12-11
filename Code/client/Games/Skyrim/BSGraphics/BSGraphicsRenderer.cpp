
#include "Systems/RenderSystemD3D11.h"
#include "World.h"

#include "BSGraphics/BSGraphicsRenderer.h"

namespace BSGraphics
{
static RenderSystemD3D11* g_sRs = nullptr;

void (*Renderer_Init)(Renderer*, BSGraphics::RendererInitOSData*, const BSGraphics::ApplicationWindowProperties*,
                      BSGraphics::RendererInitReturn*) = nullptr;

void Hook_Renderer_Init(Renderer* self, BSGraphics::RendererInitOSData* aOSData,
                        const BSGraphics::ApplicationWindowProperties* aFBData, BSGraphics::RendererInitReturn* aOut)
{
    // Append our window name.
    aOSData->pClassName = "ST Anniversary | " BUILD_BRANCH "@" BUILD_COMMIT;

    WNDPROC gameProc = aOSData->pWndProc;

    Renderer_Init(self, aOSData, aFBData, aOut);

    auto& rs = World::Get().ctx<RenderSystemD3D11>();
    g_sRs = &rs;
    // This how the game does it too
    rs.OnDeviceCreation(self->Data.RenderWindowA[0].pSwapChain);
}

void (*StopTimer)(int) = nullptr;

// Insert us at the End
void Hook_StopTimer(int type)
{
    if (g_sRs)
        g_sRs->OnRender();

    StopTimer(type);
}

template <typename T> inline T get_call(mem::pointer address)
{
    // call length
    uintptr_t target = address.as<uintptr_t>() + 5;
    target += *reinterpret_cast<int32_t*>(address.as<uintptr_t>() + 1);

    return reinterpret_cast<T>(target);
}

template <typename TFunc> void swap_call(mem::pointer p, TFunc& old_fn, const TFunc& new_fn)
{
    old_fn = get_call<TFunc>(p);
    // put new call VA
    int32_t disp = static_cast<int32_t>(reinterpret_cast<uintptr_t>(*new_fn) - p.as<uintptr_t>() - 5);

    TiltedPhoques::Put<uint8_t>(p, 0xE8);
    TiltedPhoques::Put<int>(p.as<uintptr_t>() + 1, disp);
}

static TiltedPhoques::Initializer s_viewportHooks([]() {
    // patch dwStyle in BSGraphics::InitWindows
    TiltedPhoques::Put(0x140DA38E4 + 1, WS_OVERLAPPEDWINDOW);

    // TODO: move me to input patches.
    // don't let the game steal the media keys in windowed mode
    TiltedPhoques::Put(0x140C40235 + 2, /*strip DISCL_EXCLUSIVE bits and append DISCL_NONEXCLUSIVE*/ 3);

    swap_call(0x140DA5B09, StopTimer, &Hook_StopTimer);
    swap_call(0x1405D4C2A, Renderer_Init, &Hook_Renderer_Init);
});
} // namespace BSGraphics
