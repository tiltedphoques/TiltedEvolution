#include <OverlayApp.h>
#include <filesystem>
#include <iostream>
#include <TiltedCore/Filesystem.hpp>

namespace TiltedPhoques
{
    OverlayApp::OverlayApp(RenderProvider *apRenderProvider, OverlayClient *apCustomClient, std::wstring aProcessName) noexcept
        : m_pBrowserProcessHandler(new OverlayBrowserProcessHandler)
        , m_pRenderProvider(apRenderProvider)
        , m_processName(std::move(aProcessName))
    {
        if (apCustomClient)
            m_pClient = apCustomClient;

    }

    bool OverlayApp::Initialize(const std::string_view acUri) noexcept
    {
        CefMainArgs args(GetModuleHandleW(nullptr));

        const auto currentPath = TiltedPhoques::GetPath();

        CefSettings settings;
        settings.no_sandbox = true;
        settings.multi_threaded_message_loop = true;
        settings.windowless_rendering_enabled = true;

#ifdef DEBUG
        settings.log_severity = LOGSEVERITY_VERBOSE;
        settings.remote_debugging_port = 8384;
#else
        settings.log_severity = LOGSEVERITY_VERBOSE;
#endif

        CefString(&settings.log_file).FromWString(currentPath / L"logs" / L"cef_debug.log");
        CefString(&settings.cache_path).FromWString(currentPath / L"cache");
        CefString(&settings.framework_dir_path).FromWString(currentPath);
        CefString(&settings.root_cache_path).FromWString(currentPath / L"cache");
        CefString(&settings.resources_dir_path).FromWString(currentPath);
        CefString(&settings.locales_dir_path).FromWString(currentPath / L"locales");
        CefString(&settings.browser_subprocess_path).FromWString(currentPath / m_processName);

        bool result = CefInitialize(args, settings, this, nullptr);

        if (!m_pClient)
            m_pClient = new OverlayClient(m_pRenderProvider->Create());

        CefBrowserSettings browserSettings{};

        browserSettings.windowless_frame_rate = 240;

        CefWindowInfo info;
        info.SetAsWindowless(m_pRenderProvider->GetWindow());
        //info.shared_texture_enabled = true;

        // TO BE PUT BACK
        // (currentPath / L"UI" / acPath / L"index.html").wstring()
        

        const auto ret = CefBrowserHost::CreateBrowser(info, m_pClient.get(),
            "https://www.google.com/", browserSettings, nullptr, nullptr);

        return ret;
    }

    void OverlayApp::Shutdown() noexcept
    {
        CefShutdown();
    }

    class DevToolsClient : public CefClient
    {
    private:
        IMPLEMENT_REFCOUNTING(DevToolsClient);
    };


    void OverlayApp::ShowDevTools()
    {
        if (auto browsi = m_pClient->GetBrowser())
        {
            CefWindowInfo windowInfo;
            windowInfo.SetAsPopup(nullptr, "Developer Tools");
            windowInfo.style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
            windowInfo.bounds.x = 0;
            windowInfo.bounds.y = 0;
            windowInfo.bounds.width = 640;
            windowInfo.bounds.height = 480;
            browsi->GetHost()->ShowDevTools(windowInfo, new DevToolsClient(), CefBrowserSettings(), { 0, 0 });
        }
    }

    void OverlayApp::ExecuteAsync(const std::string& acFunction, const CefRefPtr<CefListValue>& apArguments) const noexcept
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

    void OverlayApp::InjectKey(const cef_key_event_type_t aType, const uint32_t aModifiers, const uint16_t aKey, const uint16_t aScanCode) const noexcept
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

    void OverlayApp::InjectMouseButton(const uint16_t aX, const uint16_t aY, const cef_mouse_button_type_t aButton, const bool aUp, const uint32_t aModifier) const noexcept
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

    void OverlayApp::InjectMouseMove(const uint16_t aX, const uint16_t aY, const uint32_t aModifier) const noexcept
    {
        if (m_pClient && m_pClient->GetOverlayRenderHandler())
        {
            CefMouseEvent ev;

            ev.x = aX;
            ev.y = aY;
            ev.modifiers = aModifier;

            m_pClient->GetOverlayRenderHandler()->SetCursorLocation(aX, aY);

            if(m_pClient->GetBrowser() && m_pClient->GetBrowser()->GetHost())
                m_pClient->GetBrowser()->GetHost()->SendMouseMoveEvent(ev, false);
        }
    }

    void OverlayApp::InjectMouseWheel(const uint16_t aX, const uint16_t aY, const int16_t aDelta, const uint32_t aModifier) const noexcept
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

    void OverlayApp::OnBeforeCommandLineProcessing(const CefString& aProcessType, CefRefPtr<CefCommandLine> aCommandLine)
    {
        aCommandLine->AppendSwitch("allow-file-access-from-files");
        aCommandLine->AppendSwitch("allow-universal-access-from-files");
        aCommandLine->AppendSwitch("enable-gpu-rasterization");
        aCommandLine->AppendSwitch("ignore-gpu-blocklist"); // future proofing for when Google disables the above
        aCommandLine->AppendSwitch("disable-direct-composition");
        aCommandLine->AppendSwitch("disable-gpu-driver-bug-workarounds");
        aCommandLine->AppendSwitchWithValue("default-encoding", "utf-8");
        aCommandLine->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");
        aCommandLine->AppendSwitchWithValue("disable-features", "HardwareMediaKeyHandling");
        aCommandLine->AppendSwitch("disable-gpu-process-crash-limit");
        aCommandLine->AppendSwitchWithValue("use-angle", "d3d11");

        // Disable CORS
        aCommandLine->AppendSwitch("disable-web-security");
        //aCommandLine->AppendSwitch("in-process-gpu");
    }
}
