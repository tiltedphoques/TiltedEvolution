
#include <cstdint>
#include "Pch.h"
#include "RemoteControl.h"

namespace
{
// other runtimes might depend on a specific version of this runtime, so make sure to consider this when incrementing
static constexpr uint32_t kPluginVersion = 1;
} // namespace

PLUGIN_API constinit PluginAPI::PluginDescriptor TT_PLUGIN{
    .magic = PluginAPI::kPluginMagic,
    .structSize = sizeof(PluginAPI::PluginDescriptor),
    .version = kPluginVersion,
    .name = "RemoteControlV2",
    .author = "VinceM (Force67)",
    // this plugin can be reloaded at runtime. Notifications will be broadcasted via OnEvent()
    .flags = PluginAPI::PluginDescriptor::Flags::kFlagHotReload,

    // we request permission to use scripting features and get updated at runtime
    .entitlements = PluginAPI::PluginDescriptor::Entitlements::kEntScripting |
                    PluginAPI::PluginDescriptor::Entitlements::kEntUpdate,
    .pCreatePlugin = []() -> PluginAPI::IPluginInterface* { return new RemoteControlPlugin(); },
    .pDestroyPlugin =
        [](PluginAPI::IPluginInterface* apPluginInterface) {
            delete reinterpret_cast<RemoteControlPlugin*>(apPluginInterface);
        }};
