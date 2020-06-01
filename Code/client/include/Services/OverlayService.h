#pragma once

#include <include/internal/cef_ptr.h>

namespace TiltedPhoques
{
    struct OverlayApp;
}

struct RenderSystemD3D9;
struct RenderSystemD3D11;

using TiltedPhoques::OverlayApp;

struct OverlayService
{
    OverlayService();
    ~OverlayService() noexcept;

    TP_NOCOPYMOVE(OverlayService);

    void Create(RenderSystemD3D11* apRenderSystem);

    void Render() const;
    void Reset() const;

    OverlayApp* GetOverlayApp() const noexcept { return m_pOverlay.get(); }

private:

    CefRefPtr<OverlayApp> m_pOverlay{nullptr};
};
