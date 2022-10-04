
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
                                      .pluginName = "LuaScriptingRuntime",
                                      .authorName = "Vince",
                                      .flags = PluginDescriptor::Flags::kNone,
                                      .pCreatePlugin = CreateRuntime,
                                      .pDestroyPlugin = DestroyRuntime

};
