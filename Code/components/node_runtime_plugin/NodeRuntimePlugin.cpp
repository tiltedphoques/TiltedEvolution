// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#include <base64-inl.h>
#include <node_runtime_plugin/NodeRuntimePlugin.h>

#if defined(_WIN32)
#if 0
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Userenv.lib")
#endif

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "AdvAPI32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "user32.lib")

#endif



extern "C"
{
    __declspec(dllexport) server_plugins::PluginDescriptor plugin{.name = "NodeRuntime",
                                                  .desc = "NodeJS Runtime driver",
                                                  .fileName = "NodeRuntimePlugin",
        .CreatePlugin = node_runtime_plugin::NodeRuntimePlugin::Create,
        .DestroyPlugin = node_runtime_plugin::NodeRuntimePlugin::Destroy};
}

extern "C"
{

    int InitializedEmbeddedNode(const char*);
}

namespace node_runtime_plugin
{
using namespace server_plugins;

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
    InitializedEmbeddedNode("Test.exe");
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
