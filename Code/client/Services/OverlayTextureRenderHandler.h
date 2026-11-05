#pragma once

#include <OverlayRenderHandlerD3D11.hpp>

#include <atomic>

namespace DirectX
{
class SpriteBatch;
class CommonStates;
} // namespace DirectX

/**
 * @brief Draws the UI overlay with D3D11, uploading to the GPU only what CEF repainted.
 *
 * Replaces TiltedUI's OverlayRenderHandlerD3D11, which copies and uploads the whole view on every
 * CEF paint. At 5120x1440 that is about 30 MB per paint, and several paints pile up in each game frame
 * when the game runs below the UI's 60 fps, so the UI barely rendered and ignored clicks on ultrawide
 * screens, especially outdoors. Paints are merged into a CPU copy of the view on CEF's thread, and the
 * render thread uploads the changed area at most once per frame.
 */
struct OverlayTextureRenderHandler final : TiltedPhoques::OverlayRenderHandler
{
    using Renderer = TiltedPhoques::OverlayRenderHandlerD3D11::Renderer;

    explicit OverlayTextureRenderHandler(Renderer* apRenderer) noexcept;
    ~OverlayTextureRenderHandler() override;

    TP_NOCOPYMOVE(OverlayTextureRenderHandler);

    void Create() override;
    void Render() override;
    void Reset() override;

    void GetViewRect(CefRefPtr<CefBrowser> aBrowser, CefRect& aRect) override;
    void OnPaint(CefRefPtr<CefBrowser> aBrowser, PaintElementType aType, const RectList& acDirtyRects, const void* apBuffer, int aWidth, int aHeight) override;

    IMPLEMENT_REFCOUNTING(OverlayTextureRenderHandler);

private:
    // Size of the game's back buffer, or of the bound render target when there is no swap chain
    [[nodiscard]] bool GetScreenSize(uint32_t& aWidth, uint32_t& aHeight) const noexcept;
    void UpdateRenderTargetSize() noexcept;
    void CreateRenderTexture() noexcept;
    void UploadDirtyPixels() noexcept;
    // Call with m_pixelLock held
    void AddDirtyRect(uint32_t aLeft, uint32_t aTop, uint32_t aRight, uint32_t aBottom) noexcept;

    Renderer* m_pRenderer;

    // Size of the game's back buffer, which is also the size of the CEF view
    std::atomic<uint32_t> m_width{0};
    std::atomic<uint32_t> m_height{0};

    // CEF asks for the view size on a background thread before the first frame tells us the size
    std::mutex m_createLock;
    bool m_isCreateLockLocked = false;

    // CPU copy of the view, written by OnPaint on CEF's thread and uploaded by Render on the render thread
    std::mutex m_pixelLock;
    Vector<uint8_t> m_pixels;
    uint32_t m_pixelWidth{0};
    uint32_t m_pixelHeight{0};
    bool m_hasDirtyPixels = false;
    uint32_t m_dirtyLeft{0};
    uint32_t m_dirtyTop{0};
    uint32_t m_dirtyRight{0};
    uint32_t m_dirtyBottom{0};

    // Render thread only
    Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pCursorTexture;
    uint32_t m_textureWidth{0};
    uint32_t m_textureHeight{0};
    bool m_hasTextureContent = false;

    std::unique_ptr<DirectX::SpriteBatch> m_pSpriteBatch;
    std::unique_ptr<DirectX::CommonStates> m_pStates;
};
