

#include <NgClient.h>
#include <NgContextMenuHandler.h>
#include <filesystem>
#include <TiltedCore/Filesystem.hpp>

namespace TiltedPhoques
{
    /*
      virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
                             CefScreenInfo& screen_info) {
    return false;
  }
    */

    NgClient::NgClient(NgRenderHandler* apHandler, bool useSharedRes) noexcept
        : m_pRenderHandler(apHandler)
        , m_pLoadHandler(new NgLoadHandler)
        , m_pBrowser(nullptr)
        , m_enabledSharedResources(useSharedRes)
        , m_pContextMenuHandler(new NgContextMenuHandler)
    {
        const auto currentPath = TiltedPhoques::GetPath();
        m_cursorPathPNG = (currentPath / "assets" / "images" / "cursor.png").wstring();

        apHandler->SetParent(this);
    }

    void NgClient::SetBrowser(const CefRefPtr<CefBrowser>& aBrowser) noexcept
    {
        m_pBrowser = aBrowser;
    }

    void NgClient::Create() const noexcept
    {
        assert(m_pRenderHandler);

        m_pRenderHandler->Create();
    }

    void NgClient::Render() noexcept
    {
        if (m_enabledSharedResources)
            IssueFrame();

        m_pRenderHandler->Render();
    }

    void NgClient::Reset() const noexcept
    {
        assert(m_pRenderHandler);

        m_pRenderHandler->Reset();
    }

    void NgClient::IssueFrame() noexcept
    {
        // not having an instance here should crash as its a hard programmer error.
        m_pBrowser->GetHost()->SendExternalBeginFrame();
    }

    void NgClient::OnAfterCreated(CefRefPtr<CefBrowser> aBrowser)
    {
        SetBrowser(aBrowser);
    }

    void NgClient::OnBeforeClose(CefRefPtr<CefBrowser> aBrowser)
    {
        SetBrowser(nullptr);
    }

    void NgClient::AddMessageHandler(std::string key, TProcessMessageHandler handler)
    {
        m_processMessageHandlers.insert(std::make_pair(key, handler));
    }

    bool NgClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
    {
        bool success = false;

        auto it = m_processMessageHandlers.find(message->GetName());
        if (it != m_processMessageHandlers.end())
        {
            success = it->second(browser, message);
        }

        return success;
    }

    bool NgClient::IsReady() const
    {
        return m_pBrowser && m_pLoadHandler->IsReady();
    }
}
