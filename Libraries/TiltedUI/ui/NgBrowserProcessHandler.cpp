#include <NgBrowserProcessHandler.h>

namespace TiltedPhoques
{
    void NgBrowserProcessHandler::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
    {
        command_line->AppendSwitchWithValue("pid", std::to_string(GetCurrentProcessId()));
        //command_line->AppendArgument("disable-web-security");
    }
}
