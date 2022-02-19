#pragma once

#include <include/cef_v8.h>
#include <TiltedCore/Stl.hpp>

namespace TiltedPhoques
{
    using TCallbacks = Map<std::string, std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value>>>;

    struct EventsV8Handler final : CefV8Handler
    {
        explicit EventsV8Handler(CefRefPtr<CefV8Context> apContext) noexcept;
        ~EventsV8Handler() = default;

        EventsV8Handler(EventsV8Handler&&) = delete;
        EventsV8Handler(const EventsV8Handler&) = delete;
        EventsV8Handler& operator=(EventsV8Handler&&) = delete;
        EventsV8Handler& operator=(const EventsV8Handler&) = delete;

        bool Execute(const CefString& acName, CefRefPtr<CefV8Value> apObject, const CefV8ValueList& acArguments, CefRefPtr<CefV8Value>& aReturnValue, CefString& aException) override;

        const TCallbacks& GetCallbacks() const noexcept;

        IMPLEMENT_REFCOUNTING(EventsV8Handler);

    private:

        TCallbacks m_callbacks;

        CefRefPtr<CefV8Context> m_pContext;
    };
}
