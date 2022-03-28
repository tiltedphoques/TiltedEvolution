
#include <NgApp.h>
#include <TiltedCore/Filesystem.hpp>

namespace TiltedPhoques
{
    NgApp::NgApp(RenderProvider* apRenderProvider) noexcept
        : m_pBrowserProcessHandler(new NgBrowserProcessHandler)
        , m_pRenderProvider(apRenderProvider)
    {
    }

    NgApp::~NgApp() {
        // manually destruct spaces before we shutdown to 
        // work around the "reference held" error.
        for (auto& it : m_Spaces)
            it.reset();

        Shutdown();
    }

    bool NgApp::Initialize(const Settings& aSettings) noexcept
    {
        m_Settings = aSettings;

        CefSettings settings;
        settings.no_sandbox = true;
        settings.multi_threaded_message_loop = true;
        settings.windowless_rendering_enabled = true;

#ifdef DEBUG
        settings.log_severity = LOGSEVERITY_VERBOSE;
        settings.remote_debugging_port = aSettings.devtoolsPort;
#else
        //settings.log_severity = LOGSEVERITY_VERBOSE;
#endif
        const auto currentPath = TiltedPhoques::GetPath();
        CefString(&settings.log_file).FromWString(currentPath / L"logs" / L"cef_debug.log");
        CefString(&settings.cache_path).FromWString(currentPath / L"cache");
        CefString(&settings.framework_dir_path).FromWString(currentPath);
        CefString(&settings.root_cache_path).FromWString(currentPath / L"cache");
        CefString(&settings.resources_dir_path).FromWString(currentPath);
        CefString(&settings.locales_dir_path).FromWString(currentPath / L"locales");
        CefString(&settings.browser_subprocess_path).FromWString(currentPath / aSettings.pWorkerName);

        CefMainArgs args(aSettings.instanceArgs);
        return CefInitialize(args, settings, this, nullptr);
    }

    void NgApp::Shutdown() noexcept
    {
        CefShutdown();
    }

    void NgApp::OnBeforeCommandLineProcessing(const CefString& aProcessType, CefRefPtr<CefCommandLine> aCommandLine)
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

        // gpu stuff:
        aCommandLine->AppendSwitch("disable-gpu-process-crash-limit");
        aCommandLine->AppendSwitchWithValue("use-angle", "d3d11");
        aCommandLine->AppendSwitch("enable-gpu-rasterization");
        aCommandLine->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");

        // Disable CORS
        if (m_Settings.disableCors)
            aCommandLine->AppendSwitch("disable-web-security");
    }

    NgSpace* NgApp::CreateSpace(const CefString& acUrl)
    {
        assert(m_pRenderProvider);

        NgSpace* pSpaceRef = nullptr;

        auto pSpace = std::make_unique<NgSpace>();
        if (pSpace->LoadContent(m_pRenderProvider, acUrl, m_Settings.useSharedResources))
        {
            pSpaceRef = pSpace.get();
            m_Spaces.push_back(std::move(pSpace));
        }

        return pSpaceRef;
    }
}
