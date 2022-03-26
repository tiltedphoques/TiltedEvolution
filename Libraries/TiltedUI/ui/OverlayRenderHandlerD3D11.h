#pragma once

#include <wrl.h>
#include <mutex>

#include <OverlayRenderHandler.h>
#include <TiltedCore/Signal.hpp>

namespace DirectX
{
    class SpriteBatch;
    class CommonStates;
}

struct IDXGISwapChain;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11DeviceContext;
struct ID3D11Device;

namespace TiltedPhoques
{
    class OverlayRenderHandlerD3D11 final : public OverlayRenderHandler
    {
    public:
        struct Renderer
        {
            Renderer() = default;
            virtual ~Renderer() = default;
            [[nodiscard]] virtual IDXGISwapChain* GetSwapChain() const noexcept = 0;

            TP_NOCOPYMOVE(Renderer);
        };

        explicit OverlayRenderHandlerD3D11(Renderer* apRenderer) noexcept;
        virtual ~OverlayRenderHandlerD3D11();

        TP_NOCOPYMOVE(OverlayRenderHandlerD3D11);

        void Create() override;
        void Render() override;
        void Reset() override;

        void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
        void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) override;

        void OnAcceleratedPaint(
            CefRefPtr<CefBrowser> /*browser*/,
            PaintElementType type,
            const RectList& dirtyRects,
            void* share_handle) override;

        IMPLEMENT_REFCOUNTING(OverlayRenderHandlerD3D11);

    protected:

        void GetRenderTargetSize();
        void CreateRenderTexture();

    private:
        uint32_t m_width{ 1920 };
        uint32_t m_height{ 1080 };

        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pCursorTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
        std::mutex m_textureLock;
        std::mutex m_createLock;
        Renderer* m_pRenderer;

        Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;

        std::unique_ptr<DirectX::SpriteBatch> m_pSpriteBatch;
        std::unique_ptr<DirectX::CommonStates> m_pStates;
    };
}
