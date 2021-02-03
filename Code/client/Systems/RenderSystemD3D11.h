#pragma once

struct ImguiService;
struct IDXGISwapChain;
struct OverlayService;

struct RenderSystemD3D11
{
    RenderSystemD3D11(OverlayService& aOverlay, ImguiService& aImguiService);
    ~RenderSystemD3D11();

    TP_NOCOPYMOVE(RenderSystemD3D11);

    [[nodiscard]] HWND GetWindow() const;
    [[nodiscard]] IDXGISwapChain* GetSwapChain() const;

protected:

    void HandleCreate(IDXGISwapChain* apSwapChain);
    void HandleRender(IDXGISwapChain* apSwapChain);
    void HandleReset(IDXGISwapChain* apSwapChain);

private:

    IDXGISwapChain* m_pSwapChain;
    OverlayService& m_overlay;
    ImguiService& m_imguiService;
    size_t m_createConnection;
    size_t m_renderConnection;
    size_t m_resetConnection;
};
