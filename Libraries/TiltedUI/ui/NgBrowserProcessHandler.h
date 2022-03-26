#pragma once

#include <include/cef_browser_process_handler.h>

namespace TiltedPhoques
{
    class NgBrowserProcessHandler final : public CefBrowserProcessHandler
    {
    public:
        void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

        IMPLEMENT_REFCOUNTING(NgBrowserProcessHandler);
    };
}
