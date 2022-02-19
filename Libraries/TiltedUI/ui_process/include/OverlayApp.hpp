#pragma once

#include <include/cef_app.h>
#include <OverlayRenderProcessHandler.hpp>
#include <functional>

namespace TiltedPhoques
{
    struct OverlayApp final : CefApp
    {
        explicit OverlayApp(const std::function<OverlayRenderProcessHandler* ()>& aFactory) noexcept;
        ~OverlayApp() = default;

        CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

        IMPLEMENT_REFCOUNTING(OverlayApp);

    private:

        CefRefPtr<OverlayRenderProcessHandler> m_pRenderProcess;
    };

    int UIMain(const char* acpArgs, HINSTANCE aInstance, const std::function<OverlayRenderProcessHandler* ()>& acFactory) noexcept;
}
