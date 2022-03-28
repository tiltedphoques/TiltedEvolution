#include <TiltedOnlinePCH.h>

#include <Services/OverlayService.h>

#include <NgApp.h>

#include <D3D11Hook.hpp>
#include <NgRenderHandlerD3D11.h>

#include <Systems/RenderSystemD3D11.h>

#include <BSGraphics/BSGraphicsRenderer.h>
#include <World.h>

#include <Services/TransportService.h>

#define ENABLE_NG_UI 0

struct D3D11RenderProvider final : TiltedPhoques::RenderProvider,
            TiltedPhoques::NgRenderHandlerD3D11::Renderer

{
    TiltedPhoques::NgRenderHandler* Create() override
    {
        auto it = new TiltedPhoques::NgRenderHandlerD3D11(this);
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

    const NgApp::Settings config{
        .pWorkerName = L"TPProcess.exe",
        .devtoolsPort = 8384,
        .useSharedResources = true,
        .disableCors = true, // as this is an internal tool, we can disable it for development.
        .instanceArgs = hs,
    };
    if (!m_pOverlay->Initialize())
        spdlog::error("Overlay could not be initialised");

        // m_pOverlay->GetClient()->Create();
#endif
}

void OverlayService::Render() const noexcept
{
#if (ENABLE_NG_UI)
    // m_pOverlay->GetClient()->Render();
#endif
}

void OverlayService::Reset() const noexcept
{
#if (ENABLE_NG_UI)
    // m_pOverlay->GetClient()->Reset();
#endif
}
