
#include "NgClient.h"

namespace TiltedPhoques
{
    inline [[nodiscard]] CefRefPtr<NgRenderHandler> NgClient::GetOverlayRenderHandler() {
        return m_pRenderHandler;
    }

    inline [[nodiscard]] CefRefPtr<CefBrowser> NgClient::GetBrowser() const noexcept {
        return m_pBrowser;
    }

    inline [[nodiscard]] const std::wstring& NgClient::GetCursorPathPNG() const noexcept {
        return m_cursorPathPNG;
    }
}
