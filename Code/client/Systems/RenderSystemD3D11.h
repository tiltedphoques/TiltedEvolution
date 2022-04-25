#pragma once

struct ImguiService;
struct IDXGISwapChain;
struct OverlayService;

/**
* @brief Runs the renderer for D3D11.
*/
struct RenderSystemD3D11
{
    RenderSystemD3D11(OverlayService& aOverlay, ImguiService& aImguiService);
    ~RenderSystemD3D11();

    TP_NOCOPYMOVE(RenderSystemD3D11);

    [[nodiscard]] HWND GetWindow() const;
    [[nodiscard]] IDXGISwapChain* GetSwapChain() const;

    // to make yamashi mad
    void OnDeviceCreation(IDXGISwapChain* apSwapChain);
    void OnRender();
    void OnReset(IDXGISwapChain* apSwapChain);

private:

    IDXGISwapChain* m_pSwapChain;
    OverlayService& m_overlay;
    ImguiService& m_imguiService;
    size_t m_createConnection;
    size_t m_renderConnection;
    size_t m_resetConnection;
};
