#pragma once

#include <include/cef_load_handler.h>
#include <TiltedCore/Meta.hpp>

namespace TiltedPhoques
{
    class NgLoadHandler final : public CefLoadHandler
    {
    public:
        NgLoadHandler() = default;
        virtual ~NgLoadHandler() = default;

        void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) override;
        void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
        void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) override;

        [[nodiscard]] bool IsReady() const noexcept { return m_ready; }

        TP_NOCOPYMOVE(NgLoadHandler);
        IMPLEMENT_REFCOUNTING(NgLoadHandler);

    private:
        bool m_ready{ false };
    };
}
