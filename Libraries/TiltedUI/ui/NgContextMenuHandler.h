#pragma once
#include <include/cef_context_menu_handler.h>

#include <TiltedCore/Meta.hpp>

namespace TiltedPhoques
{
    class NgContextMenuHandler final : public CefContextMenuHandler
    {
    public:
        NgContextMenuHandler() = default;
        virtual ~NgContextMenuHandler() = default;

        void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) override;

        TP_NOCOPYMOVE(NgContextMenuHandler);
        IMPLEMENT_REFCOUNTING(NgContextMenuHandler);
    };
}
