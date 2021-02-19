#pragma once

#include <OverlayRenderProcessHandler.hpp>

struct ProcessHandler : TiltedPhoques::OverlayRenderProcessHandler
{
    ProcessHandler() noexcept;
    virtual ~ProcessHandler() = default;

    TP_NOCOPYMOVE(ProcessHandler);

    void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
    void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
};
