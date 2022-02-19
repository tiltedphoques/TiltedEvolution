#include <OverlayContextHandler.hpp>

namespace TiltedPhoques
{
    void OverlayContextHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
    {
        model->Clear();
    }
}
