#pragma once

#include "OverlayClient.hpp"

struct TransportService;

namespace TiltedPhoques
{
struct OverlayRenderHandler;
}

/**
 * @brief Renders the UI overlay.
 */
struct OverlayClient : TiltedPhoques::OverlayClient
{
    OverlayClient(TransportService& aTransport, TiltedPhoques::OverlayRenderHandler* apHandler);
    virtual ~OverlayClient() noexcept;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

    TP_NOCOPYMOVE(OverlayClient);

private:
    void ProcessConnectMessage(CefRefPtr<CefListValue> aEventArgs);
    void ProcessDisconnectMessage();
    void ProcessRevealPlayersMessage();
    void ProcessChatMessage(CefRefPtr<CefListValue> aEventArgs);
    void ProcessSetTimeCommand(CefRefPtr<CefListValue> aEventArgs);
    void ProcessTeleportMessage(CefRefPtr<CefListValue> aEventArgs);
    void ProcessToggleDebugUI();
    void SetUIVisible(bool aVisible) noexcept;

    TransportService& m_transport;
};
