
#include <SpriteBatch.h>
#include <DirectXColors.h>
#include <SimpleMath.h>
#include <CommonStates.h>

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include <OverlayClient.hpp>
#include <OverlayRenderHandlerD3D11.hpp>

namespace TiltedPhoques
{
    OverlayRenderHandlerD3D11::OverlayRenderHandlerD3D11(Renderer* apRenderer) noexcept
        : m_pRenderer(apRenderer)
    {
        // So we need to lock this until we have the window dimension as a background CEF thread will attempt to get it before we have it
        m_createLock.lock();
    }

    OverlayRenderHandlerD3D11::~OverlayRenderHandlerD3D11() = default;

    void OverlayRenderHandlerD3D11::Render()
    {
        // We need contexts first
        if (!m_pImmediateContext || !m_pContext)
        {
            Create();

            if (!m_pImmediateContext || !m_pContext)
                return;
        }

        // First of all we flush our deferred context in case we have updated the texture
        {
            std::unique_lock<std::mutex> _(m_textureLock);

            Microsoft::WRL::ComPtr<ID3D11CommandList> pCommandList;
            const auto result = m_pContext->FinishCommandList(FALSE, &pCommandList);

            if (result == S_OK && pCommandList)
            {
                m_pImmediateContext->ExecuteCommandList(pCommandList.Get(), TRUE);
            }
        }

        GetRenderTargetSize();

        if (IsVisible())
        {
            m_pSpriteBatch->Begin(DirectX::SpriteSortMode_Deferred, m_pStates->NonPremultiplied());

            {
                std::unique_lock<std::mutex> _(m_textureLock);

                if (m_pTextureView)
                    m_pSpriteBatch->Draw(m_pTextureView.Get(), DirectX::SimpleMath::Vector2(0.f, 0.f), nullptr, DirectX::Colors::White, 0.f);
            }

            if (m_pCursorTexture)
            {
                m_pSpriteBatch->Draw(m_pCursorTexture.Get(), DirectX::SimpleMath::Vector2(m_cursorX, m_cursorY), nullptr, DirectX::Colors::White, 0.f, DirectX::SimpleMath::Vector2(0, 0), m_width / 1920.f);
            }

            m_pSpriteBatch->End();
        }
    }

    void OverlayRenderHandlerD3D11::Reset()
    {
        Create();
    }

    void OverlayRenderHandlerD3D11::Create()
    {
        const auto hr = m_pRenderer->GetSwapChain()->GetDevice(IID_ID3D11Device, reinterpret_cast<void**>(m_pDevice.ReleaseAndGetAddressOf()));

        if (FAILED(hr))
            return;

        m_pDevice->GetImmediateContext(m_pImmediateContext.ReleaseAndGetAddressOf());

        if (!m_pImmediateContext)
            return;

        GetRenderTargetSize();

        if (FAILED(m_pDevice->CreateDeferredContext(0, m_pContext.ReleaseAndGetAddressOf())))
            return;

        m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(m_pImmediateContext.Get());
        m_pStates = std::make_unique<DirectX::CommonStates>(m_pDevice.Get());

        if (FAILED(DirectX::CreateWICTextureFromFile(m_pDevice.Get(), m_pParent->GetCursorPathPNG().c_str(), nullptr, m_pCursorTexture.ReleaseAndGetAddressOf())))
        {
            DirectX::CreateDDSTextureFromFile(m_pDevice.Get(), m_pParent->GetCursorPathDDS().c_str(), nullptr, m_pCursorTexture.ReleaseAndGetAddressOf());
        }

        std::unique_lock<std::mutex> _(m_textureLock);

        if (!m_pTexture)
            CreateRenderTexture();
    }

    void OverlayRenderHandlerD3D11::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
    {
        std::scoped_lock _(m_createLock);

        rect = CefRect(0, 0, m_width, m_height);
    }

    void OverlayRenderHandlerD3D11::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
        const RectList& dirtyRects, const void* buffer, int width, int height)
    {
        if (type == PET_VIEW && m_width == width && m_height == height)
        {
            std::unique_lock<std::mutex> _(m_textureLock);

            if (!m_pTexture)
                CreateRenderTexture();

            D3D11_MAPPED_SUBRESOURCE mappedResource;
            const auto result = m_pContext->Map(m_pTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

            if(SUCCEEDED(result))
            {
                const auto pDest = static_cast<uint8_t*>(mappedResource.pData);
                std::memcpy(pDest, buffer, width * height * 4);
                m_pContext->Unmap(m_pTexture.Get(), 0);
            }
            else
            {
                // We got no mapping, let's drop the context and reset the texture so that we attempt to create a new one during the next frame
                m_pContext.Reset();
                m_pTexture.Reset();
                m_width = m_height = 0;
            }
        }
    }

    void OverlayRenderHandlerD3D11::GetRenderTargetSize()
    {
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;

        m_pImmediateContext->OMGetRenderTargets(1, pRenderTargetView.ReleaseAndGetAddressOf(), nullptr);
        if (pRenderTargetView)
        {
            Microsoft::WRL::ComPtr<ID3D11Resource> pSrcResource;
            pRenderTargetView->GetResource(pSrcResource.ReleaseAndGetAddressOf());

            if (pSrcResource)
            {
                Microsoft::WRL::ComPtr<ID3D11Texture2D> pSrcBuffer;
                pSrcResource.As(&pSrcBuffer);

                D3D11_TEXTURE2D_DESC desc;
                pSrcBuffer->GetDesc(&desc);

                if ((m_width != desc.Width || m_height != desc.Height) && m_pParent)
                {
                    m_width = desc.Width;
                    m_height = desc.Height;

                    // We now know the size of the viewport, we can let CEF get it
                    m_createLock.unlock();

                    {
                        std::unique_lock<std::mutex> _(m_textureLock);

                        m_pTexture.Reset();
                        m_pTextureView.Reset();
                    }

                    if (m_pParent->GetBrowser())
                        m_pParent->GetBrowser()->GetHost()->WasResized();
                }
            }
        }
    }

    void OverlayRenderHandlerD3D11::CreateRenderTexture()
    {
        D3D11_TEXTURE2D_DESC textDesc;
        textDesc.Width = m_width;
        textDesc.Height = m_height;
        textDesc.MipLevels = textDesc.ArraySize = 1;
        textDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        textDesc.SampleDesc.Count = 1;
        textDesc.SampleDesc.Quality = 0;
        textDesc.Usage = D3D11_USAGE_DYNAMIC;
        textDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        textDesc.MiscFlags = 0;

        if (FAILED(m_pDevice->CreateTexture2D(&textDesc, nullptr, m_pTexture.ReleaseAndGetAddressOf())))
            return;

        D3D11_SHADER_RESOURCE_VIEW_DESC sharedResourceViewDesc = {};
        sharedResourceViewDesc.Format = textDesc.Format;
        sharedResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        sharedResourceViewDesc.Texture2D.MipLevels = 1;

        if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture.Get(), &sharedResourceViewDesc, m_pTextureView.ReleaseAndGetAddressOf())))
            return;
    }
}
