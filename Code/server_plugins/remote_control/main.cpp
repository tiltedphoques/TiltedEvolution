
#include <cstdint>
#include "Pch.h"
#include "RemoteControl.h"

namespace
{
// other runtimes might depend on a specific version of this runtime, so make sure to consider this when incrementing
static constexpr uint32_t kPluginVersion = 1;
} // namespace

PLUGIN_API constinit PluginDescriptor TT_PLUGIN{
    .magic = kPluginMagic,
    .structSize = sizeof(PluginDescriptor),
    .version = kPluginVersion,
    .name = "RemoteControlV2",
    .author = "VinceM (Force67)",
    // this plugin can be reloaded at runtime. Notifications will be broadcasted via OnEvent()
    .flags = PluginDescriptor::Flags::kFlagHotReload,

    // we request permission to use scripting features and get updated at runtime
    .entitlements = PluginDescriptor::Entitlements::kEntScripting | PluginDescriptor::Entitlements::kEntUpdate,
    .pCreatePlugin = []() -> IPluginInterface* { return new RemoteControlPlugin(); },
    .pDestroyPlugin =
        [](IPluginInterface* apPluginInterface) { delete reinterpret_cast<RemoteControlPlugin*>(apPluginInterface); }};
