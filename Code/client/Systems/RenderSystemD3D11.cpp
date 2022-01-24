#include <TiltedOnlinePCH.h>

#include <Systems/RenderSystemD3D11.h>

#include <Services/OverlayService.h>
#include <Services/ImguiService.h>

#include <D3D11Hook.hpp>
#include <d3d11.h>


RenderSystemD3D11::RenderSystemD3D11(OverlayService& aOverlay, ImguiService& aImguiService)
    : m_pSwapChain(nullptr)
    , m_overlay(aOverlay)
    , m_imguiService(aImguiService)
{
    auto& d3d11 = TiltedPhoques::D3D11Hook::Get();

    m_createConnection = d3d11.OnCreate.Connect(std::bind(&RenderSystemD3D11::OnDeviceCreation, this, std::placeholders::_1));
    m_resetConnection = d3d11.OnLost.Connect(std::bind(&RenderSystemD3D11::OnReset, this, std::placeholders::_1));

    //m_renderConnection = d3d11.OnPresent.Connect(std::bind(&RenderSystemD3D11::OnRender, this, std::placeholders::_1));
}

RenderSystemD3D11::~RenderSystemD3D11()
{
    auto& d3d11 = TiltedPhoques::D3D11Hook::Get();

    d3d11.OnCreate.Disconnect(m_createConnection);
   // d3d11.OnPresent.Disconnect(m_renderConnection);
    d3d11.OnLost.Disconnect(m_resetConnection);
}

HWND RenderSystemD3D11::GetWindow() const
{
    DXGI_SWAP_CHAIN_DESC desc{};
    desc.OutputWindow = nullptr;

    if(m_pSwapChain)
        m_pSwapChain->GetDesc(&desc);

    return desc.OutputWindow;
}

IDXGISwapChain* RenderSystemD3D11::GetSwapChain() const
{
    return m_pSwapChain;
}

void RenderSystemD3D11::OnDeviceCreation(IDXGISwapChain* apSwapChain)
{
    m_pSwapChain = apSwapChain;

    m_imguiService.Create(this, GetWindow());
    m_overlay.Create(this);
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
