#pragma once
#include <include/cef_context_menu_handler.h>

#include <TiltedCore/Meta.hpp>

namespace TiltedPhoques
{
    struct OverlayContextHandler final : CefContextMenuHandler
    {
        OverlayContextHandler() = default;
        virtual ~OverlayContextHandler() = default;

        TP_NOCOPYMOVE(OverlayContextHandler);

        void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) override;

        IMPLEMENT_REFCOUNTING(OverlayContextHandler);
    };
}
