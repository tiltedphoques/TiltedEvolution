
#include "PythonRuntime.h"
#include "Pch.h"

// This is a great resource that i used while embedding python:
// https://pybind11.readthedocs.io/en/stable/advanced/embedding.html
PythonRuntime::~PythonRuntime()
{
}

bool PythonRuntime::Initialize()
{
    WriteLog(LogLevel::kInfo, "Initialized python runtime\n");
    return true;
}

void PythonRuntime::Shutdown()
{
    WriteLog(LogLevel::kInfo, "Shutting down python\n");
}

void PythonRuntime::CallScriptFunction(const PluginStringView aName, ScriptFunctionContext& aContext)
{
}

void PythonRuntime::BindScriptFunction(const PluginStringView aName, void* apFunctor, const ArgType* apArgs,
                                       const size_t aArgCount)
{ // todo: lock
}
