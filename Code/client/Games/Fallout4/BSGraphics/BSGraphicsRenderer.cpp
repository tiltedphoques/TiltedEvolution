#include "BSGraphicsRenderer.h"

void Hook_Renderer_Init(BSGraphics::Renderer* self, BSGraphics::RendererInitOSData* aOSData,
                        const BSGraphics::ApplicationWindowProperties* aFBData, BSGraphics::RendererInitReturn* aOut)
{
    /*
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
    */
}
