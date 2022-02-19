#pragma once

#include <include/cef_browser_process_handler.h>

namespace TiltedPhoques
{
    struct OverlayBrowserProcessHandler : CefBrowserProcessHandler
    {
        void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

        IMPLEMENT_REFCOUNTING(OverlayBrowserProcessHandler);
    };
}
