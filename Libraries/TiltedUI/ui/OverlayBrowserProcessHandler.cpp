#include <OverlayBrowserProcessHandler.hpp>

namespace TiltedPhoques
{
    void OverlayBrowserProcessHandler::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
    {
        command_line->AppendSwitchWithValue("pid", std::to_string(GetCurrentProcessId()));
        command_line->AppendArgument("disable-web-security");
    }
}
