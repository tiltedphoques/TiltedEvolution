
#include "Pch.h"
#include "PythonRuntime.h"

namespace
{
// other runtimes might depend on a specific version of this runtime, so make sure to consider this when incrementing
static constexpr uint32_t kPluginVersion = 1;

// we only support .lua files
static const char* kExtension = ".py";
constinit PluginAPI::ScriptInfoBlock kScriptInfoBlock{
    .supportedExtensionCount = 1,
    .supportedExtensions = &kExtension,
};
} // namespace

PLUGIN_API constinit PluginAPI::PluginDescriptor TT_PLUGIN{
    .magic = PluginAPI::kPluginMagic,
    .structSize = sizeof(PluginAPI::PluginDescriptor),
    .version = kPluginVersion,
    .name = "PythonScriptingRuntime",
    .author = "VinceM (Force67)",
    .infoblocks =
        {
            // how we will handle scripts
            {.magic = PluginAPI::ScriptInfoBlock::kMagic,
             .structSize = sizeof(PluginAPI::ScriptInfoBlock),
             .ptr = &kScriptInfoBlock},
        },
    // this plugin can be reloaded at runtime. Notifications will be broadcasted via OnEvent()
    .flags = PluginAPI::PluginDescriptor::Flags::kFlagHotReload,

    // we request permission to use scripting features and get updated at runtime
    .entitlements = PluginAPI::PluginDescriptor::Entitlements::kEntScripting |
                    PluginAPI::PluginDescriptor::Entitlements::kEntUpdate,
    .pCreatePlugin = []() -> PluginAPI::IPluginInterface* { return new PythonRuntime(); },
    .pDestroyPlugin =
        [](PluginAPI::IPluginInterface* apPluginInterface) {
            delete reinterpret_cast<PythonRuntime*>(apPluginInterface);
        }};
