#include <TiltedOnlinePCH.h>

#include <OverlayRenderHandler.hpp>

#include <Services/OverlayClient.h>
#include <Services/TransportService.h>

#include <Events/CommandEvent.h>

#include <Messages/SendChatMessageRequest.h>

#include <World.h>

OverlayClient::OverlayClient(TransportService& aTransport, TiltedPhoques::OverlayRenderHandler* apHandler)
    : TiltedPhoques::OverlayClient(apHandler), m_transport(aTransport)
{
}

OverlayClient::~OverlayClient() noexcept
{
}

bool OverlayClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                             CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    if (message->GetName() == "ui-event")
    {
        auto pArguments = message->GetArgumentList();

        auto eventName = pArguments->GetString(0).ToString();
        auto eventArgs = pArguments->GetList(1);

        spdlog::info(eventName);
        spdlog::info(eventArgs->GetString(0).ToString());
        spdlog::info(std::to_string(eventArgs->GetInt(1)));
        spdlog::info(eventArgs->GetString(2).ToString());

#ifndef PUBLIC_BUILD
        LOG(INFO) << "event=ui_event name=" << eventName;
#endif

        // TODO: this stuff should really be delegated
        // at least into different OverlayClient funcs, but maybe even
        // dispatch events for OverlayService to catch
        if (eventName == "connect")
        {
            std::string baseIp = eventArgs->GetString(0);
            if (baseIp == "localhost")
            {
                baseIp = "127.0.0.1";
            }

            uint16_t port = eventArgs->GetInt(1) ? eventArgs->GetInt(1) : 10578;
            m_transport.Connect(baseIp + ":" + std::to_string(port));
            // iAmAToken = eventArgs->GeString(2);
        }
        else if (eventName == "disconnect")
        {
            m_transport.Close();
        }
        else if (eventName == "sendMessage")
        {
            std::string contents = eventArgs->GetString(0).ToString();
            if (!contents.empty())
            {
                if (contents[0] == '/')
                {
                    World::Get().GetRunner().Trigger(CommandEvent(std::move(String(contents))));
                }
                else
                {
                    SendChatMessageRequest messageRequest;
                    messageRequest.ChatMessage = eventArgs->GetString(0).ToString();
                    spdlog::debug("Received Message from UI and will send it to server: " + messageRequest.ChatMessage);
                    m_transport.Send(messageRequest);
                }
            }
        }

        return true;
    }

    return false;
}
