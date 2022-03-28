#pragma once

#include <NgBrowserProcessHandler.h>
#include <NgClient.h>
#include <NgSpace.h>

#include <TiltedCore/Meta.hpp>
#include <include/cef_app.h>

namespace TiltedPhoques
{
    struct RenderProvider
    {
        RenderProvider() = default;
        virtual ~RenderProvider() = default;
        virtual NgRenderHandler* Create() = 0;
        virtual HWND GetWindow() = 0;

        TP_NOCOPYMOVE(RenderProvider);
    };

    class NgApp final : public CefApp
    {
    public:
        explicit NgApp(RenderProvider* apRenderProvider) noexcept;
        virtual ~NgApp();

        struct Settings {
            const wchar_t* pWorkerName;
            int devtoolsPort;
            bool useSharedResources;
            bool disableCors;
            HINSTANCE instanceArgs;
        };
        bool Initialize(const Settings& aCreateInfo) noexcept;
        void Shutdown() noexcept;

        NgSpace* CreateSpace(const CefString& acUrl);

        TP_NOCOPYMOVE(NgApp);
        IMPLEMENT_REFCOUNTING(NgApp);

    private:
        // CefApp
        void OnBeforeCommandLineProcessing(const CefString& aProcessType, CefRefPtr<CefCommandLine> aCommandLine) override;
        CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return m_pBrowserProcessHandler; }

    private:
        CefRefPtr<NgBrowserProcessHandler> m_pBrowserProcessHandler;
        RenderProvider* m_pRenderProvider;
        std::vector<std::unique_ptr<NgSpace>> m_Spaces;
        Settings m_Settings{};
    };
}
