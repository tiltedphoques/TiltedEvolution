// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <node_runtime_plugin/NodeRuntimePlugin.h>

namespace node_runtime_plugin
{
using namespace server_plugins;

extern "C"
{
    __declspec(dllexport) PluginDescriptor plugin{.name = "NodeRuntime",
                                                  .desc = "NodeJS Runtime driver",
                                                  .fileName = "NodeRuntimePlugin",
                                                  .createPlugin = NodeRuntimePlugin::Create,
                                                  .destroyPlugin = NodeRuntimePlugin::Destroy};
}

// Regular plugin traits.
Plugin* NodeRuntimePlugin::Create()
{
    return new NodeRuntimePlugin();
}

void NodeRuntimePlugin::Destroy(Plugin* apPlugin)
{
    delete apPlugin;
}

void NodeRuntimePlugin::Init()
{
    // CreateNodeJS driver
}

void NodeRuntimePlugin::Update()
{
    // Tick NodeJS driver
}

void NodeRuntimePlugin::Detatch()
{
    // Destroy NodeJS driver.
}

} // namespace node_runtime_plugin
