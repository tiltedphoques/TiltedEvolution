#pragma once

#include <algorithm>

#include <include/cef_render_process_handler.h>

#include <OverlayV8Handler.hpp>
#include <EventsV8Handler.hpp>

namespace TiltedPhoques
{
    struct OverlayRenderProcessHandler : CefRenderProcessHandler
    {
        explicit OverlayRenderProcessHandler(std::string aCoreObjectName = "core") noexcept;
        ~OverlayRenderProcessHandler() = default;

        OverlayRenderProcessHandler(OverlayRenderProcessHandler&&) = delete;
        OverlayRenderProcessHandler(const OverlayRenderProcessHandler&) = delete;

        OverlayRenderProcessHandler& operator=(const OverlayRenderProcessHandler&) = delete;
        OverlayRenderProcessHandler& operator=(OverlayRenderProcessHandler&&) = delete;

        void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
        void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefProcessId sourceProcess,
            CefRefPtr<CefProcessMessage> message) override final;

        void CreateEvent(const std::string& acEventName) const;
        void CreateFunction(const std::string& acFunctionName) const;

        IMPLEMENT_REFCOUNTING(OverlayRenderProcessHandler);

    protected:

        static CefRefPtr<CefV8Value> ConvertValue(const CefRefPtr<CefValue>& apValue);

        CefRefPtr<EventsV8Handler> m_pEventsHandler;
        CefRefPtr<OverlayV8Handler> m_pOverlayHandler;
        CefRefPtr<CefV8Value> m_pCoreObject;
        std::string m_coreObjectName;
    };
}
