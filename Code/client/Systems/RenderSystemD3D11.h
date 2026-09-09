#pragma once

struct ImguiService;
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct OverlayService;

/**
 * @brief Runs the renderer for D3D11.
 */
struct RenderSystemD3D11
{
    RenderSystemD3D11(OverlayService& aOverlay, ImguiService& aImguiService);
    ~RenderSystemD3D11() = default;

    TP_NOCOPYMOVE(RenderSystemD3D11);

    [[nodiscard]] HWND GetWindow() const;
    [[nodiscard]] IDXGISwapChain* GetSwapChain() const { return m_pSwapChain; };
    [[nodiscard]] ID3D11Device* GetDevice() const { return m_pDevice; };
    [[nodiscard]] ID3D11DeviceContext* GetDeviceContext() const { return m_pDeviceContext; };

    // to make yamashi mad
    void OnDeviceCreation(IDXGISwapChain* apSwapChain, ID3D11Device* apDevice, ID3D11DeviceContext* apContext);
    void OnRender();
    void OnReset(IDXGISwapChain* apSwapChain);

private:
    IDXGISwapChain* m_pSwapChain;
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pDeviceContext;

    OverlayService& m_overlay;
    ImguiService& m_imguiService;
    size_t m_createConnection;
    size_t m_renderConnection;
    size_t m_resetConnection;
};
