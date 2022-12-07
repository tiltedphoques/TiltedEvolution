
#include "Pch.h"
#include "PythonRuntime.h"

namespace
{
PluginInterface001* CreateRuntime()
{
    return new PythonRuntime();
}

void DestroyRuntime(PluginInterface001* interface)
{
    delete interface; // WARNING: your dtor will be not invoked if you not override it.
}
} // namespace

PLUGIN_API PluginDescriptor TT_PLUGIN{.magic = kPluginMagic,
                                      .structSize = sizeof(PluginDescriptor),
                                      .pluginVersion = 1,
                                      .pluginName = "PythonScriptingRuntime",
                                      .authorName = "VinceM (Force67)",
                                      .flags = PluginDescriptor::Flags::kHotReload,
                                      .pCreatePlugin = CreateRuntime,
                                      .pDestroyPlugin = DestroyRuntime

};
