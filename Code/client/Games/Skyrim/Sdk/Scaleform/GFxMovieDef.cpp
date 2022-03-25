
#include "Sdk/Scaleform/GFxMovieDef.h"
#include "BSGraphics/BSGraphicsRenderer.h"

namespace Scaleform::GFx
{
Render::RectF GetFrameRectFromHeader(const MovieHeaderData& header)
{
    return TwipsToPixels(header.FrameRect);
}

void (*MovieDataDef_LoadTaskData_BeginSWFLoading)(MovieDataDef::LoadTaskData*, MovieHeaderData*){nullptr};

// Learn about how the games handles frame resizing here:
// https://www.nexusmods.com/fallout4/articles/149

void Hook_MovieDataDef_LoadTaskData_BeginSWFLoading(MovieDataDef::LoadTaskData* pSelf, MovieHeaderData* apHeader)
{
    Render::RectF translate{GetFrameRectFromHeader(*apHeader)};

    if (strstr(&pSelf->FileURL.pData->Data[0], "Map.swf"))
    {
        auto* pState = BSGraphics::State::Get();

        // need to patch the other code stuff aswell...
        apHeader->FrameRect.x1 = static_cast<float>((pState->uiBackBufferWidth / 2.f) / 2.f) * 20.f;
        apHeader->FrameRect.x2 = static_cast<float>(pState->uiBackBufferWidth) * 20.f;
        apHeader->FrameRect.y2 = static_cast<float>(pState->uiBackBufferHeight) * 20.f;
    }

    // uncap menu FPS.
    apHeader->FPS = 60.f;

    MovieDataDef_LoadTaskData_BeginSWFLoading(pSelf, apHeader);
}

static TiltedPhoques::Initializer s_InitMovie([]() {
    const VersionDbPtr<uint8_t> callSite(87361);

    TiltedPhoques::SwapCall(callSite.Get() + 0x41, MovieDataDef_LoadTaskData_BeginSWFLoading,
                            &Hook_MovieDataDef_LoadTaskData_BeginSWFLoading);
});
} // namespace Scaleform::GFx
