#pragma once

#include "OverlayClient.hpp"

struct TransportService;

namespace TiltedPhoques
{
struct OverlayRenderHandler;
}

struct OverlayClient : public TiltedPhoques::OverlayClient
{

  public:
    OverlayClient(TransportService& aTransport, TiltedPhoques::OverlayRenderHandler* apHandler);
    ~OverlayClient() noexcept;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override;

    TP_NOCOPYMOVE(OverlayClient);

  private:
    TransportService& m_transport;
};
