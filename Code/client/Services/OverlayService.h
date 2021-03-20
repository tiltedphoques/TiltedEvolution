#pragma once

#include <include/internal/cef_ptr.h>

namespace TiltedPhoques
{
    struct OverlayApp;
}

struct RenderSystemD3D11;
struct D3D11RenderProvider;
struct World;
struct TransportService;

using TiltedPhoques::OverlayApp;

struct OverlayService
{
  public:
    OverlayService(World& aWorld, TransportService& transport, entt::dispatcher& aDispatcher);
    ~OverlayService() noexcept;

    TP_NOCOPYMOVE(OverlayService);

    void Create(RenderSystemD3D11* apRenderSystem) noexcept;

    void Render() const noexcept;
    void Reset() const noexcept;

    OverlayApp* GetOverlayApp() const noexcept { return m_pOverlay.get(); }

private:

    CefRefPtr<OverlayApp> m_pOverlay{nullptr};
    TiltedPhoques::UniquePtr<D3D11RenderProvider> m_pProvider;

    World& m_world;
    TransportService& m_transport;
};
