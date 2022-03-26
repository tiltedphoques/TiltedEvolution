#include <TiltedOnlinePCH.h>

#include <Services/OverlayService.h>

#include <OverlayApp.h>

#include <D3D11Hook.hpp>
#include <OverlayRenderHandlerD3D11.h>

#include <Systems/RenderSystemD3D11.h>

#include <BSGraphics/BSGraphicsRenderer.h>
#include <World.h>

#include <Services/OverlayClient.h>
#include <Services/TransportService.h>

using TiltedPhoques::OverlayRenderHandler;
using TiltedPhoques::OverlayRenderHandlerD3D11;

#define ENABLE_NG_UI 1

struct D3D11RenderProvider final : TiltedPhoques::RenderProvider, OverlayRenderHandlerD3D11::Renderer
{
    OverlayRenderHandler* Create() override
    {
        auto it = new OverlayRenderHandlerD3D11(this);
        it->SetVisible(true);
        return it;
    }

    HWND GetWindow() override
    {
        return BSGraphics::GetMainWindow()->hWnd;
    }

    IDXGISwapChain* GetSwapChain() const noexcept override
    {
        return BSGraphics::GetMainWindow()->pSwapChain;
    }
};

OverlayService::OverlayService(World& aWorld, TransportService& transport, entt::dispatcher& aDispatcher)
    : m_world(aWorld), m_transport(transport)
{
}

OverlayService::~OverlayService() noexcept
{
}

void OverlayService::Create(RenderSystemD3D11* apRenderSystem) noexcept
{
#if (ENABLE_NG_UI)
    m_pProvider = TiltedPhoques::MakeUnique<D3D11RenderProvider>();
    m_pOverlay = new OverlayApp(m_pProvider.get(), new ::OverlayClient(m_transport, m_pProvider->Create()));

    if (!m_pOverlay->Initialize())
        spdlog::error("Overlay could not be initialised");

    //m_pOverlay->GetClient()->Create();
#endif
}

void OverlayService::Render() const noexcept
{
#if (ENABLE_NG_UI)
    //m_pOverlay->GetClient()->Render();
#endif
}

void OverlayService::Reset() const noexcept
{
#if (ENABLE_NG_UI)
    //m_pOverlay->GetClient()->Reset();
#endif
}
