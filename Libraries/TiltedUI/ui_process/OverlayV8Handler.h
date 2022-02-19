#pragma once

#include <include/cef_v8.h>

namespace TiltedPhoques
{
    struct OverlayV8Handler final : CefV8Handler
    {
        explicit OverlayV8Handler(const CefRefPtr<CefBrowser>& apBrowser) noexcept;
        ~OverlayV8Handler() = default;

        bool Execute(const CefString& acName, CefRefPtr<CefV8Value> apObject, const CefV8ValueList& acArguments, CefRefPtr<CefV8Value>& aReturnValue, CefString& aException) override;

        IMPLEMENT_REFCOUNTING(OverlayV8Handler);

    private:

        void Dispatch(const CefString& acName, const CefV8ValueList& acArguments) const noexcept;

        static CefRefPtr<CefValue> ConvertValue(const CefRefPtr<CefV8Value>& acpValue);

        CefRefPtr<CefBrowser> m_pBrowser;
    };
}
