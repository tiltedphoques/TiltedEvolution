
#include "LuaRuntime.h"

namespace
{
PluginInterface001* CreateRuntime()
{
    return new LuaRuntime();
}

void DestroyRuntime(PluginInterface001* interface)
{
    delete interface; // WARNING: your dtor will be not invoked if you not override it.
}
} // namespace

PLUGIN_API PluginDescriptor TT_PLUGIN{.magic = kPluginMagic,
                                      .structSize = sizeof(PluginDescriptor),
                                      .pluginVersion = 1,
                                      .pluginName = "LuaScriptingRuntime",
                                      .authorName = "Vince",
                                      .flags = PluginDescriptor::Flags::kNone,
                                      .pCreatePlugin = CreateRuntime,
                                      .pDestroyPlugin = DestroyRuntime

};
