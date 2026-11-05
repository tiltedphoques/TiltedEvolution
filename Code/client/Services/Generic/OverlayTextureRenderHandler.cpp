#include <TiltedOnlinePCH.h>

#include <Services/OverlayTextureRenderHandler.h>

#include <OverlayClient.hpp>

#include <d3d11.h>

#include <algorithm>
#include <cstring>

#include <CommonStates.h>
#include <DDSTextureLoader.h>
#include <DirectXColors.h>
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <WICTextureLoader.h>

// CEF paints BGRA, 4 bytes per pixel
static constexpr uint32_t kBytesPerPixel = 4;
// The cursor image is drawn at its own size on a 1080 pixels tall screen
static constexpr float kCursorReferenceHeight = 1080.f;

OverlayTextureRenderHandler::OverlayTextureRenderHandler(Renderer* apRenderer) noexcept
    : m_pRenderer(apRenderer)
{
    // Held until the first frame tells us the view size, GetViewRect() waits on it
    m_createLock.lock();
    m_isCreateLockLocked = true;
}

OverlayTextureRenderHandler::~OverlayTextureRenderHandler() = default;

void OverlayTextureRenderHandler::Create()
{
    m_pDevice = m_pRenderer->GetDevice();
    m_pImmediateContext = m_pRenderer->GetDeviceContext();

    if (!m_pDevice || !m_pImmediateContext)
        return;

    UpdateRenderTargetSize();

    m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(m_pImmediateContext.Get());
    m_pStates = std::make_unique<DirectX::CommonStates>(m_pDevice.Get());

    if (FAILED(DirectX::CreateWICTextureFromFile(m_pDevice.Get(), m_pParent->GetCursorPathPNG().c_str(), nullptr, m_pCursorTexture.ReleaseAndGetAddressOf())))
    {
        DirectX::CreateDDSTextureFromFile(m_pDevice.Get(), m_pParent->GetCursorPathDDS().c_str(), nullptr, m_pCursorTexture.ReleaseAndGetAddressOf());
    }

    CreateRenderTexture();
}

void OverlayTextureRenderHandler::Render()
{
    if (!m_pImmediateContext || !m_pSpriteBatch)
    {
        Create();

        if (!m_pImmediateContext || !m_pSpriteBatch)
            return;
    }

    UpdateRenderTargetSize();

    if (!m_pTexture)
        CreateRenderTexture();

    UploadDirtyPixels();

    if (!IsVisible())
        return;

    m_pSpriteBatch->Begin(DirectX::SpriteSortMode_Deferred, m_pStates->NonPremultiplied());

    if (m_pTextureView && m_hasTextureContent)
        m_pSpriteBatch->Draw(m_pTextureView.Get(), DirectX::SimpleMath::Vector2(0.f, 0.f), nullptr, DirectX::Colors::White, 0.f);

    if (IsCursorVisible() && m_pCursorTexture)
    {
        // Scaled with the height so the cursor keeps its size on ultrawide screens
        const float cursorScale = static_cast<float>(m_height) / kCursorReferenceHeight;
        m_pSpriteBatch->Draw(m_pCursorTexture.Get(), DirectX::SimpleMath::Vector2(m_cursorX, m_cursorY), nullptr, DirectX::Colors::White, 0.f, DirectX::SimpleMath::Vector2(0.f, 0.f), cursorScale);
    }

    m_pSpriteBatch->End();
}

void OverlayTextureRenderHandler::Reset()
{
    m_pTexture.Reset();
    m_pTextureView.Reset();
    m_pCursorTexture.Reset();
    m_pDevice.Reset();
    m_pImmediateContext.Reset();
    m_textureWidth = 0;
    m_textureHeight = 0;
    m_hasTextureContent = false;

    m_pSpriteBatch = nullptr;
    m_pStates = nullptr;

    Create();
}

void OverlayTextureRenderHandler::GetViewRect(CefRefPtr<CefBrowser> aBrowser, CefRect& aRect)
{
    std::scoped_lock _(m_createLock);

    aRect = CefRect(0, 0, static_cast<int>(m_width.load()), static_cast<int>(m_height.load()));
}

void OverlayTextureRenderHandler::OnPaint(CefRefPtr<CefBrowser> aBrowser, PaintElementType aType, const RectList& acDirtyRects, const void* apBuffer, int aWidth, int aHeight)
{
    // A paint of the previous size is dropped, CEF paints the whole view again after WasResized()
    if (aType != PET_VIEW || aWidth <= 0 || aHeight <= 0 || static_cast<uint32_t>(aWidth) != m_width || static_cast<uint32_t>(aHeight) != m_height)
        return;

    const uint8_t* pSource = static_cast<const uint8_t*>(apBuffer);
    const size_t rowSize = static_cast<size_t>(aWidth) * kBytesPerPixel;

    std::scoped_lock _(m_pixelLock);

    if (m_pixelWidth != static_cast<uint32_t>(aWidth) || m_pixelHeight != static_cast<uint32_t>(aHeight))
    {
        m_pixels.resize(rowSize * static_cast<size_t>(aHeight));
        m_pixelWidth = static_cast<uint32_t>(aWidth);
        m_pixelHeight = static_cast<uint32_t>(aHeight);

        std::memcpy(m_pixels.data(), pSource, m_pixels.size());
        AddDirtyRect(0, 0, m_pixelWidth, m_pixelHeight);
        return;
    }

    for (const CefRect& dirtyRect : acDirtyRects)
    {
        const int left = std::clamp(dirtyRect.x, 0, aWidth);
        const int top = std::clamp(dirtyRect.y, 0, aHeight);
        const int right = std::clamp(dirtyRect.x + dirtyRect.width, left, aWidth);
        const int bottom = std::clamp(dirtyRect.y + dirtyRect.height, top, aHeight);

        if (left == right || top == bottom)
            continue;

        const size_t columnOffset = static_cast<size_t>(left) * kBytesPerPixel;
        const size_t copySize = static_cast<size_t>(right - left) * kBytesPerPixel;

        for (int row = top; row < bottom; ++row)
        {
            const size_t rowStart = static_cast<size_t>(row) * rowSize + columnOffset;
            std::memcpy(m_pixels.data() + rowStart, pSource + rowStart, copySize);
        }

        AddDirtyRect(static_cast<uint32_t>(left), static_cast<uint32_t>(top), static_cast<uint32_t>(right), static_cast<uint32_t>(bottom));
    }
}

void OverlayTextureRenderHandler::UpdateRenderTargetSize() noexcept
{
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;

    m_pImmediateContext->OMGetRenderTargets(1, pRenderTargetView.ReleaseAndGetAddressOf(), nullptr);
    if (!pRenderTargetView)
        return;

    Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
    pRenderTargetView->GetResource(pResource.ReleaseAndGetAddressOf());

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pRenderTarget;
    if (!pResource || FAILED(pResource.As(&pRenderTarget)))
        return;

    D3D11_TEXTURE2D_DESC description;
    pRenderTarget->GetDesc(&description);

    if ((m_width == description.Width && m_height == description.Height) || !m_pParent)
        return;

    m_width = description.Width;
    m_height = description.Height;

    // We now know the size of the view, CEF can get it
    if (m_isCreateLockLocked)
    {
        m_createLock.unlock();
        m_isCreateLockLocked = false;
    }

    m_pTexture.Reset();
    m_pTextureView.Reset();
    m_hasTextureContent = false;

    if (m_pParent->GetBrowser())
        m_pParent->GetBrowser()->GetHost()->WasResized();
}

void OverlayTextureRenderHandler::CreateRenderTexture() noexcept
{
    const uint32_t width = m_width;
    const uint32_t height = m_height;

    if (!m_pDevice || width == 0 || height == 0)
        return;

    // Default usage: the texture is only written with UpdateSubresource(), which keeps the pixels
    // outside the updated box, unlike mapping a dynamic texture with D3D11_MAP_WRITE_DISCARD
    D3D11_TEXTURE2D_DESC textureDescription{};
    textureDescription.Width = width;
    textureDescription.Height = height;
    textureDescription.MipLevels = 1;
    textureDescription.ArraySize = 1;
    textureDescription.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDescription.SampleDesc.Count = 1;
    textureDescription.SampleDesc.Quality = 0;
    textureDescription.Usage = D3D11_USAGE_DEFAULT;
    textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDescription.CPUAccessFlags = 0;
    textureDescription.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&textureDescription, nullptr, m_pTexture.ReleaseAndGetAddressOf())))
        return;

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDescription{};
    viewDescription.Format = textureDescription.Format;
    viewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    viewDescription.Texture2D.MipLevels = 1;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture.Get(), &viewDescription, m_pTextureView.ReleaseAndGetAddressOf())))
    {
        m_pTexture.Reset();
        return;
    }

    m_textureWidth = width;
    m_textureHeight = height;
    m_hasTextureContent = false;

    // The new texture is empty, so the whole view goes up on the next upload
    std::scoped_lock _(m_pixelLock);

    if (m_pixelWidth == width && m_pixelHeight == height)
        AddDirtyRect(0, 0, width, height);
}

void OverlayTextureRenderHandler::UploadDirtyPixels() noexcept
{
    if (!m_pTexture)
        return;

    std::scoped_lock _(m_pixelLock);

    // While the view is being resized, keep the dirty area until CEF paints at the texture's size
    if (!m_hasDirtyPixels || m_pixelWidth != m_textureWidth || m_pixelHeight != m_textureHeight)
        return;

    const uint32_t rowSize = m_pixelWidth * kBytesPerPixel;
    const uint8_t* pSource = m_pixels.data() + static_cast<size_t>(m_dirtyTop) * rowSize + static_cast<size_t>(m_dirtyLeft) * kBytesPerPixel;
    const D3D11_BOX box{m_dirtyLeft, m_dirtyTop, 0, m_dirtyRight, m_dirtyBottom, 1};

    m_pImmediateContext->UpdateSubresource(m_pTexture.Get(), 0, &box, pSource, rowSize, 0);

    m_hasDirtyPixels = false;
    m_hasTextureContent = true;
}

void OverlayTextureRenderHandler::AddDirtyRect(uint32_t aLeft, uint32_t aTop, uint32_t aRight, uint32_t aBottom) noexcept
{
    if (!m_hasDirtyPixels)
    {
        m_dirtyLeft = aLeft;
        m_dirtyTop = aTop;
        m_dirtyRight = aRight;
        m_dirtyBottom = aBottom;
        m_hasDirtyPixels = true;
        return;
    }

    m_dirtyLeft = std::min(m_dirtyLeft, aLeft);
    m_dirtyTop = std::min(m_dirtyTop, aTop);
    m_dirtyRight = std::max(m_dirtyRight, aRight);
    m_dirtyBottom = std::max(m_dirtyBottom, aBottom);
}
