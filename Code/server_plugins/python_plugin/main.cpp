
#include "Pch.h"
#include "PythonRuntime.h"

namespace
{
// other runtimes might depend on a specific version of this runtime, so make sure to consider this when incrementing
static constexpr uint32_t kPluginVersion = 1;

// we only support .lua files
static const char* kExtension = ".py";
constinit ScriptInfoBlock kScriptInfoBlock{
    .supportedExtensionCount = 1,
    .supportedExtensions = &kExtension,
};
} // namespace

PLUGIN_API constinit PluginDescriptor TT_PLUGIN{
    .magic = kPluginMagic,
    .structSize = sizeof(PluginDescriptor),
    .version = kPluginVersion,
    .name = "PythonScriptingRuntime",
    .author = "VinceM (Force67)",
    .infoblocks =
        {
            // how we will handle scripts
            {.magic = ScriptInfoBlock::kMagic, .structSize = sizeof(ScriptInfoBlock), .ptr = &kScriptInfoBlock},
        },
    // this plugin can be reloaded at runtime. Notifications will be broadcasted via OnEvent()
    .flags = PluginDescriptor::Flags::kFlagHotReload,

    // we request permission to use scripting features and get updated at runtime
    .entitlements = PluginDescriptor::Entitlements::kEntScripting | PluginDescriptor::Entitlements::kEntUpdate,
    .pCreatePlugin = []() -> IPluginInterface* { return new PythonRuntime(); },
    .pDestroyPlugin =
        [](IPluginInterface* apPluginInterface) { delete reinterpret_cast<PythonRuntime*>(apPluginInterface); }};
