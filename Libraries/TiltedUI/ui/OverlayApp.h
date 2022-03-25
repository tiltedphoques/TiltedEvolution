#pragma once

#include <OverlayBrowserProcessHandler.h>
#include <OverlayClient.h>
#include <OverlaySpace.h>

#include <TiltedCore/Meta.hpp>
#include <include/cef_app.h>

namespace TiltedPhoques
{
    class OverlaySpace;

    struct RenderProvider
    {
        RenderProvider() = default;
        virtual ~RenderProvider() = default;
        virtual OverlayRenderHandler* Create() = 0;
        virtual HWND GetWindow() = 0;

        TP_NOCOPYMOVE(RenderProvider);
    };

    class OverlayApp : public CefApp
    {
    public:

       
        explicit OverlayApp(RenderProvider* apRenderProvider, OverlayClient* apCustomClient, std::wstring aProcessName = L"TPProcess.exe") noexcept;
        virtual ~OverlayApp();

        TP_NOCOPYMOVE(OverlayApp);

        bool Initialize() noexcept;
        void Shutdown() noexcept;

        OverlaySpace* CreateSpace();

        void OnBeforeCommandLineProcessing(const CefString& aProcessType, CefRefPtr<CefCommandLine> aCommandLine) override;

        CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return m_pBrowserProcessHandler; }

        IMPLEMENT_REFCOUNTING(OverlayApp);

    private:

        CefRefPtr<OverlayBrowserProcessHandler> m_pBrowserProcessHandler;
        RenderProvider* m_pRenderProvider;
        std::wstring m_processName;
        std::vector<OverlaySpace*> m_Spaces;
    };
}
