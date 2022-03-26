#include <NgContextMenuHandler.h>

namespace TiltedPhoques
{
    void NgContextMenuHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
    {
        model->Clear();
    }
}
