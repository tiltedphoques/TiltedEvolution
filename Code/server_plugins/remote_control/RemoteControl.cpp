
#include "RemoteControl.h"
#include "Pch.h"

// This is a great resource that i used while embedding python:
// https://pybind11.readthedocs.io/en/stable/advanced/embedding.html
RemoteControlPlugin::~RemoteControlPlugin()
{
}

bool RemoteControlPlugin::Initialize()
{

    WriteLog(LogLevel::kInfo, "Initialized remote control plugin\n");
    return true;
}

void RemoteControlPlugin::Shutdown()
{
    WriteLog(LogLevel::kInfo, "Shutting down remote control plugin\n");
}

void RemoteControlPlugin::CallScriptFunction(const PluginStringView aName, ScriptFunctionContext& aContext)
{
}

void RemoteControlPlugin::BindScriptFunction(const PluginStringView aName, void* apFunctor, const ArgType* apArgs,
                                             const size_t aArgCount)
{ // todo: lock
}
