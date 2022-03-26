#pragma once

#include <include/cef_client.h>
#include <NgLoadHandler.h>
#include <OverlayRenderHandler.h>

#include <TiltedCore/Meta.hpp>

namespace TiltedPhoques
{
    class NgClient final : public CefClient, CefLifeSpanHandler
    {
    public:
        explicit NgClient(OverlayRenderHandler* apHandler, bool useSharedRes) noexcept;

        inline [[nodiscard]] CefRefPtr<OverlayRenderHandler> GetOverlayRenderHandler();
        inline [[nodiscard]] CefRefPtr<CefBrowser> GetBrowser() const noexcept;
        inline [[nodiscard]] const std::wstring& GetCursorPathPNG() const noexcept;

        void Render() noexcept;
        void Create() const noexcept;
        void Reset() const noexcept;

        void IssueFrame() noexcept;

        using TProcessMessageHandler = std::function<bool(CefRefPtr<CefBrowser>, CefRefPtr<CefProcessMessage>)>;
        void AddMessageHandler(std::string key, TProcessMessageHandler handler);

        bool IsReady() const;

        TP_NOCOPYMOVE(NgClient);
        IMPLEMENT_REFCOUNTING(NgClient);

    private:
        void OnAfterCreated(CefRefPtr<CefBrowser> aBrowser) override;
        void OnBeforeClose(CefRefPtr<CefBrowser> aBrowser) override;
        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

        void SetBrowser(const CefRefPtr<CefBrowser>& aBrowser) noexcept;

        CefRefPtr<CefRenderHandler> GetRenderHandler() override {
            return m_pRenderHandler;
        }

        CefRefPtr<CefLoadHandler> GetLoadHandler() override {
            return m_pLoadHandler;
        }

        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
            return this;
        }

        CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override {
            return m_pContextMenuHandler;
        }
    private:
        CefRefPtr<OverlayRenderHandler> m_pRenderHandler;
        CefRefPtr<NgLoadHandler> m_pLoadHandler;
        CefRefPtr<CefBrowser> m_pBrowser;
        CefRefPtr<CefContextMenuHandler> m_pContextMenuHandler;
        std::map<std::string, TProcessMessageHandler> m_processMessageHandlers;
        std::wstring m_cursorPathPNG;
        bool m_enabledSharedResources{ false };
    };
}

#include "NgClient.inl"
