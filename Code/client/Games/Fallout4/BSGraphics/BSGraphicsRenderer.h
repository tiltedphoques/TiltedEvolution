#pragma once

#include <d3d11.h>

namespace BSGraphics
{

struct BSCriticalSection
{
    _RTL_CRITICAL_SECTION CriticalSection;
};

static_assert(sizeof(BSCriticalSection) == 0x28);

struct RenderTarget
{
    ID3D11Texture2D *pTexture;
    ID3D11Texture2D *pCopyTexture;
    ID3D11RenderTargetView *pRTView;
    ID3D11ShaderResourceView *pSRView;
    ID3D11ShaderResourceView *pCopySRView;
    ID3D11UnorderedAccessView *pUAView;
};

static_assert(sizeof(RenderTarget) == 0x30);

struct RendererWindow
{
    HWND__ *hWnd;
    int iWindowX;
    int iWindowY;
    int uiWindowWidth;
    int uiWindowHeight;
    IDXGISwapChain *pSwapChain;
    RenderTarget SwapChainRenderTarget;

    bool IsForeground();
};

static_assert(sizeof(RendererWindow) == 0x50);

struct DepthStencilTarget
{
    ID3D11Texture2D *pTexture;
    ID3D11DepthStencilView *pDSView[4];
    ID3D11DepthStencilView *pDSViewReadOnlyDepth[4];
    ID3D11DepthStencilView *pDSViewReadOnlyStencil[4];
    ID3D11DepthStencilView *pDSViewReadOnlyDepthStencil[4];
    ID3D11ShaderResourceView *pSRViewDepth;
    ID3D11ShaderResourceView *pSRViewStencil;
};

static_assert(sizeof(DepthStencilTarget) == 0x98);

struct CubeMapRenderTarget
{
    ID3D11Texture2D *pTexture;
    ID3D11RenderTargetView *pRTView[6];
    ID3D11ShaderResourceView *pSRView;
};

static_assert(sizeof(CubeMapRenderTarget) == 0x40);

struct __declspec(align(8)) RendererData
{
    void* pShadowState;
    BSD3DResourceCreator* D3DResourceCreator;
    uint32_t uiAdapter;
    DXGI_RATIONAL DesiredRefreshRate;
    DXGI_RATIONAL ActualRefreshRate;
    DXGI_MODE_SCALING ScaleMode;
    DXGI_MODE_SCANLINE_ORDER ScanlineMode;
    int bFullScreen;
    bool bAppFullScreen;
    bool bBorderlessWindow;
    bool bVSync;
    bool bInitialized;
    bool bRequestWindowSizeChange;
    unsigned int uiNewWidth;
    unsigned int uiNewHeight;
    unsigned int uiPresentInterval;
    ID3D11Device *pDevice;
    ID3D11DeviceContext *pContext;
    RendererWindow RenderWindowA[32];
    RenderTarget pRenderTargetsA[101];
    DepthStencilTarget pDepthStencilTargetsA[13];
    CubeMapRenderTarget pCubeMapRenderTargetsA[2];
    BSCriticalSection RendererLock;
    const char *pClassName;
    HINSTANCE__ *hInstance;
    bool bNVAPIEnabled;
};

static_assert(sizeof(RendererData) == 0x25C0);

struct Renderer
{
    bool bSkipNextPresent;
    void (__fastcall *ResetRenderTargets)();
    RendererData Data;
};

static_assert(sizeof(Renderer) == 0x25D0);

struct RendererInitOSData
{
    HWND__ *hWnd;
    HINSTANCE__ *hInstance;
    __int64 (__fastcall *pWndProc)(HWND__ *, unsigned int, unsigned __int64, __int64);
    HICON__ *hIcon;
    const char *pClassName;
    unsigned int uiAdapter;
    int bCreateSwapChainRenderTarget;
};

static_assert(sizeof(RendererInitOSData) == 0x30);

struct ApplicationWindowProperties
{
    unsigned int uiWidth;
    unsigned int uiHeight;
    int iX;
    int iY;
    unsigned int uiRefreshRate;
    bool bFullScreen;
    bool bBorderlessWindow;
    bool bVSync;
    unsigned int uiPresentInterval;
};

static_assert(sizeof(ApplicationWindowProperties) == 0x1C);

struct RendererInitReturn
{
    HWND__ *hwnd;
};

static_assert(sizeof(RendererInitReturn) == 0x8);

RendererWindow* GetMainWindow();

}
