#pragma once

#include <include/cef_client.h>
#include <OverlayLoadHandler.h>
#include <OverlayRenderHandler.hpp>

#include <TiltedCore/Meta.hpp>

namespace TiltedPhoques
{
    struct OverlayClient : CefClient, CefLifeSpanHandler, CefContextMenuHandler
    {
        explicit OverlayClient(OverlayRenderHandler* apHandler) noexcept;

        TP_NOCOPYMOVE(OverlayClient);

        [[nodiscard]] CefRefPtr<OverlayRenderHandler> GetOverlayRenderHandler();
        CefRefPtr<CefRenderHandler> GetRenderHandler() override;
        CefRefPtr<CefLoadHandler> GetLoadHandler() override;
        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
        CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;

        [[nodiscard]] CefRefPtr<CefBrowser> GetBrowser() const noexcept;
        [[nodiscard]] const std::wstring& GetCursorPathPNG() const noexcept;
        [[nodiscard]] const std::wstring& GetCursorPathDDS() const noexcept;

        void Render() const noexcept;
        void Create() const noexcept;
        void Reset() const noexcept;

        void OnAfterCreated(CefRefPtr<CefBrowser> aBrowser) override;
        void OnBeforeClose(CefRefPtr<CefBrowser> aBrowser) override;
        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

        bool IsReady() const;

        IMPLEMENT_REFCOUNTING(OverlayClient);

    private:

        void SetBrowser(const CefRefPtr<CefBrowser>& aBrowser) noexcept;

        CefRefPtr<OverlayRenderHandler> m_pRenderHandler;
        CefRefPtr<OverlayLoadHandler> m_pLoadHandler;
        CefRefPtr<CefBrowser> m_pBrowser;
        CefRefPtr<CefContextMenuHandler> m_pContextMenuHandler;

        std::wstring m_cursorPathPNG;
        std::wstring m_cursorPathDDS;
    };
}
