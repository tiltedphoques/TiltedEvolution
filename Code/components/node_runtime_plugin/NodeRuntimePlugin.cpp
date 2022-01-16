// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <node.h>
#include <node_runtime_plugin/NodeRuntimePlugin.h>
#include <v8-initialization.h>
#include <v8.h>
#include <uv.h>

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


#include <string>
#include <vector>


extern "C"
{
    __declspec(dllexport) server_plugins::PluginDescriptor plugin{
        .name = "NodeRuntime",
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
using namespace v8;
using namespace node;

// https://nodejs.org/api/embedding.html
int RunNodeInstance(MultiIsolatePlatform* platform, const std::vector<std::string>& args,
                    const std::vector<std::string>& exec_args)
{
    int exit_code = 0;

    // Setup up a libuv event loop, v8::Isolate, and Node.js Environment.
    std::vector<std::string> errors;
    std::unique_ptr<CommonEnvironmentSetup> setup = CommonEnvironmentSetup::Create(platform, &errors, args, exec_args);
    if (!setup)
    {
        for (const std::string& err : errors)
            fprintf(stderr, "%s: %s\n", args[0].c_str(), err.c_str());
        return 1;
    }

    Isolate* isolate = setup->isolate();
    Environment* env = setup->env();

    {
        Locker locker(isolate);
        Isolate::Scope isolate_scope(isolate);
        // The v8::Context needs to be entered when node::CreateEnvironment() and
        // node::LoadEnvironment() are being called.
        Context::Scope context_scope(setup->context());

        // Set up the Node.js instance for execution, and run code inside of it.
        // There is also a variant that takes a callback and provides it with
        // the `require` and `process` objects, so that it can manually compile
        // and run scripts as needed.
        // The `require` function inside this script does *not* access the file
        // system, and can only load built-in Node.js modules.
        // `module.createRequire()` is being used to create one that is able to
        // load files from the disk, and uses the standard CommonJS file loader
        // instead of the internal-only `require` function.
        MaybeLocal<Value> loadenv_ret =
            node::LoadEnvironment(env, "const publicRequire ="
                                       "  require('module').createRequire(process.cwd() + '/');"
                                       "globalThis.require = publicRequire;"
                                       "require('vm').runInThisContext(process.argv[1]);");

        if (loadenv_ret.IsEmpty()) // There has been a JS exception.
            return 1;

        exit_code = node::SpinEventLoop(env).FromMaybe(1);

        // node::Stop() can be used to explicitly stop the event loop and keep
        // further JavaScript from running. It can be called from any thread,
        // and will act like worker.terminate() if called from another thread.
        node::Stop(env);
    }

    return exit_code;
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

bool NodeRuntimePlugin::Init()
{
    char** cv = new char* [10] {};
    *cv = (char*)R"(S:\Work\Tilted\TiltedEvolution\build\windows\x64\debug\SkyrimTogetherServer.exe)";

    auto argv = uv_setup_args(1, cv);
    //std::vector<std::string> args{R"(S:\Work\Tilted\TiltedEvolution\build\windows\x64\debug\SkyrimTogetherServer.exe)"};
    std::vector<std::string> args(argv, argv + 1);
    std::vector<std::string> exec_args;
    std::vector<std::string> errors;
    // Parse Node.js CLI options, and print any errors that have occurred while
    // trying to parse them.
    int exitCode = node::InitializeNodeWithArgs(&args, &exec_args, &errors);

    //int exitCode = InitializedEmbeddedNode("Test.exe");
    if (exitCode != 0)
    {
        // TODO: log.
        return false;
    }

    auto platform = node::MultiIsolatePlatform::Create(4);
    V8::InitializePlatform(platform.get());
    V8::Initialize();

    int ret = RunNodeInstance(platform.get(), args, exec_args);

    return true;
}

void NodeRuntimePlugin::Update()
{
    // Tick NodeJS driver
}

void NodeRuntimePlugin::Detatch()
{
    V8::Dispose();
    V8::ShutdownPlatform();
}

} // namespace node_runtime_plugin
