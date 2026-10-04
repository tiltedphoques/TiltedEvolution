#pragma once

struct IDXGISwapChain;

namespace BSGraphics
{
struct RendererWindow
{
    HWND hWnd;
    int32_t iWindowX;
    int32_t iWindowY;
    int32_t uiWindowWidth;
    int32_t uiWindowHeight;
    IDXGISwapChain* pSwapChain;

    bool IsForeground();
};

static_assert(offsetof(RendererWindow, pSwapChain) == 0x18);

RendererWindow* GetMainWindow();
}
