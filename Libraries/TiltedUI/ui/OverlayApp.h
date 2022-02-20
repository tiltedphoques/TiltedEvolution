#pragma once

#include <OverlayBrowserProcessHandler.hpp>
#include <OverlayClient.h>

#include <TiltedCore/Meta.hpp>
#include <include/cef_app.h>

namespace TiltedPhoques
{
    struct OverlayApp : CefApp
    {
        struct RenderProvider
        {
            RenderProvider() = default;
            virtual ~RenderProvider() = default;
            virtual OverlayRenderHandler* Create() = 0;
            virtual HWND GetWindow() = 0;

            TP_NOCOPYMOVE(RenderProvider);
        };

        explicit OverlayApp(RenderProvider *apRenderProvider, OverlayClient *apCustomClient, std::wstring aProcessName = L"TPProcess.exe") noexcept;
        virtual ~OverlayApp() = default;

        TP_NOCOPYMOVE(OverlayApp);

        bool Initialize(const std::string_view acUri) noexcept;
        void Shutdown() noexcept;

        void ExecuteAsync(const std::string& acFunction, const CefRefPtr<CefListValue>& apArguments = nullptr) const noexcept;

        [[nodiscard]] OverlayClient* GetClient() const noexcept { return m_pClient.get(); };

        void InjectKey(cef_key_event_type_t aType, uint32_t aModifiers, uint16_t aKey, uint16_t aScanCode) const noexcept;
        void InjectMouseButton(uint16_t aX, uint16_t aY, cef_mouse_button_type_t aButton, bool aUp, uint32_t aModifier) const noexcept;
        void InjectMouseMove(uint16_t aX, uint16_t aY, uint32_t aModifier) const noexcept;
        void InjectMouseWheel(uint16_t aX, uint16_t aY, int16_t aDelta, uint32_t aModifier) const noexcept;

        void OnBeforeCommandLineProcessing(const CefString& aProcessType, CefRefPtr<CefCommandLine> aCommandLine) override;

        CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return m_pBrowserProcessHandler; }

        IMPLEMENT_REFCOUNTING(OverlayApp);

    private:

        CefRefPtr<OverlayBrowserProcessHandler> m_pBrowserProcessHandler;
        CefRefPtr<OverlayClient> m_pClient;
        RenderProvider* m_pRenderProvider;
        std::wstring m_processName;
    };
}
