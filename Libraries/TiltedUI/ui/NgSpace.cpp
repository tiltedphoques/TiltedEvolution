
#include "NgSpace.h"
#include "NgApp.h"

namespace TiltedPhoques
{
    namespace
    {
        class DevToolsClient : public CefClient
        {
        private:
            IMPLEMENT_REFCOUNTING(DevToolsClient);
        };
    }
    // ActivateContent
    void NgSpace::LoadContent(RenderProvider* apRenderer, bool enableSharedResources)
    {
        m_usingSharedResources = enableSharedResources;
        m_pClient = new NgClient(apRenderer->Create(), enableSharedResources);

        CefBrowserSettings settings{};
        settings.javascript_close_windows = STATE_DISABLED;
        settings.windowless_frame_rate = 240;

        CefWindowInfo info;
        info.SetAsWindowless(apRenderer->GetWindow());
        info.shared_texture_enabled = enableSharedResources;
        info.external_begin_frame_enabled = enableSharedResources;

        // TO BE PUT BACK
        // (currentPath / L"UI" / acPath / L"index.html").wstring()

        bool ret = CefBrowserHost::CreateBrowser(info, m_pClient.get(),
            "https://www.google.com/", settings, nullptr, nullptr);

        if (ret)
        {
            m_pClient->Create();
        }
    }

    bool NgSpace::ShowDevTools()
    {
        if (!m_pClient->GetBrowser())
            return false;

        auto browserRef{ m_pClient->GetBrowser() };

        CefWindowInfo windowInfo;
        windowInfo.SetAsPopup(nullptr, "Developer Tools");
        windowInfo.style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
        windowInfo.bounds.x = 0;
        windowInfo.bounds.y = 0;
        windowInfo.bounds.width = 640;
        windowInfo.bounds.height = 480;
        browserRef->GetHost()->ShowDevTools(windowInfo, new DevToolsClient(), CefBrowserSettings(), { 0, 0 });
        return true;
    }

    void NgSpace::ExecuteAsync(const std::string& acFunction, const CefRefPtr<CefListValue>& apArguments) const noexcept
    {
        if (!m_pClient)
            return;

        auto pMessage = CefProcessMessage::Create("browser-event");
        auto pArguments = pMessage->GetArgumentList();

        const auto pFunctionArguments = apArguments ? apArguments : CefListValue::Create();

        pArguments->SetString(0, acFunction);
        pArguments->SetList(1, pFunctionArguments);

        auto pBrowser = m_pClient->GetBrowser();
        if (pBrowser)
        {
            pBrowser->GetMainFrame()->SendProcessMessage(PID_RENDERER, pMessage);
        }
    }

    void NgSpace::InjectKey(const cef_key_event_type_t aType, const uint32_t aModifiers, const uint16_t aKey, const uint16_t aScanCode) const noexcept
    {
        if (m_pClient)
        {
            CefKeyEvent ev;

            ev.type = aType;
            ev.modifiers = aModifiers;
            ev.windows_key_code = aKey;
            ev.native_key_code = aScanCode;

            if (m_pClient->GetBrowser() && m_pClient->GetBrowser()->GetHost())
                m_pClient->GetBrowser()->GetHost()->SendKeyEvent(ev);
        }
    }

    void NgSpace::InjectMouseButton(const uint16_t aX, const uint16_t aY, const cef_mouse_button_type_t aButton, const bool aUp, const uint32_t aModifier) const noexcept
    {
        if (m_pClient)
        {
            CefMouseEvent ev;

            ev.x = aX;
            ev.y = aY;
            ev.modifiers = aModifier;

            if (m_pClient->GetBrowser() && m_pClient->GetBrowser()->GetHost())
                m_pClient->GetBrowser()->GetHost()->SendMouseClickEvent(ev, aButton, aUp, 1);
        }
    }

    void NgSpace::InjectMouseMove(const uint16_t aX, const uint16_t aY, const uint32_t aModifier) const noexcept
    {
        if (m_pClient && m_pClient->GetOverlayRenderHandler())
        {
            CefMouseEvent ev;

            ev.x = aX;
            ev.y = aY;
            ev.modifiers = aModifier;

            m_pClient->GetOverlayRenderHandler()->SetCursorLocation(aX, aY);

            if (m_pClient->GetBrowser() && m_pClient->GetBrowser()->GetHost())
                m_pClient->GetBrowser()->GetHost()->SendMouseMoveEvent(ev, false);
        }
    }

    void NgSpace::InjectMouseWheel(const uint16_t aX, const uint16_t aY, const int16_t aDelta, const uint32_t aModifier) const noexcept
    {
        if (m_pClient && m_pClient->GetOverlayRenderHandler())
        {
            CefMouseEvent ev;

            ev.x = aX;
            ev.y = aY;
            ev.modifiers = aModifier;

            if (m_pClient->GetBrowser() && m_pClient->GetBrowser()->GetHost())
                m_pClient->GetBrowser()->GetHost()->SendMouseWheelEvent(ev, 0, aDelta);
        }
    }
}
