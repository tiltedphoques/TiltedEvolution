#include <TiltedOnlinePCH.h>

#include <Systems/RenderSystemD3D11.h>

#include <Services/DebugService.h>
#include <Services/OverlayService.h>
#include <Services/ImguiService.h>

#include <d3d11.h>

RenderSystemD3D11::RenderSystemD3D11(OverlayService& aOverlay, ImguiService& aImguiService)
    : m_pSwapChain(nullptr)
    , m_pDevice(nullptr)
    , m_pDeviceContext(nullptr)
    , m_overlay(aOverlay)
    , m_imguiService(aImguiService)
{
    // Note: D3D11Hook is not utilized in the codebase

    // auto& d3d11 = TiltedPhoques::D3D11Hook::Get();
    // m_createConnection = d3d11.OnCreate.Connect(std::bind(&RenderSystemD3D11::OnDeviceCreation, this, std::placeholders::_2));
    // m_resetConnection = d3d11.OnLost.Connect(std::bind(&RenderSystemD3D11::OnReset, this, std::placeholders::_1));
    // m_renderConnection = d3d11.OnPresent.Connect(std::bind(&RenderSystemD3D11::OnRender, this, std::placeholders::_1));
}

HWND RenderSystemD3D11::GetWindow() const
{
    DXGI_SWAP_CHAIN_DESC desc{};
    desc.OutputWindow = nullptr;

    if (m_pSwapChain)
        m_pSwapChain->GetDesc(&desc);

    return desc.OutputWindow;
}

void RenderSystemD3D11::OnDeviceCreation(IDXGISwapChain* apSwapChain, ID3D11Device* apDevice, ID3D11DeviceContext* apContext)
{
    m_pSwapChain = apSwapChain;
    m_pDevice = apDevice;
    m_pDeviceContext = apContext;

    m_imguiService.Create(this, GetWindow());
    m_overlay.Create(this);
    DebugService::ArrangeGameWindows(GetWindow());
}

void RenderSystemD3D11::OnRender()
{
    m_imguiService.Render();
    m_overlay.Render();
}

void RenderSystemD3D11::OnReset(IDXGISwapChain* apSwapChain)
{
    m_pSwapChain = apSwapChain;

    m_overlay.Reset();
    m_imguiService.Reset();
}
