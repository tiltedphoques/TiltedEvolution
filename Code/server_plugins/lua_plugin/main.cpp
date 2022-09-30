
#include "LuaRuntime.h"

PluginInterface001* CreateRuntime()
{
    return new LuaRuntime();
}

void DestroyRuntime(PluginInterface001* interface)
{
    delete interface; // WARNING: your dtor will be not invoked if you not override it.
}

PLUGIN_API PluginDescriptor TT_PLUGIN{.magic = kPluginMagic,
                                   .pluginVersion = 1,
                                   .pName = "LuaScriptingRuntime",
                                   .pAuthorName = "Vince",
                                   .flags = PluginDescriptor::Flags::kNone,
                                   .CreatePlugin = CreateRuntime,
                                   .DestroyPlugin = DestroyRuntime

};

#ifdef _WIN32
#include <Windows.h>
// don't do anything in here.
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif
