// Copyright (C) Vincent Hengel 2022

#include "PythonRuntime.h"
#include "Pch.h"
#include "PythonEnvironment.h"
#include "PythonFunction.h"

#include <PluginAPI/Logging.h>

namespace PythonScripting
{
namespace py = pybind11;

// This is a great resource that i used while embedding python:
// https://pybind11.readthedocs.io/en/stable/advanced/embedding.html
PythonRuntime::~PythonRuntime() noexcept
{
}

bool PythonRuntime::Initialize()
{
    // clear these predefined env variables so that python doesn't try to use them
    (void)putenv("PYTHONPATH=");
    (void)putenv("PYTHONHOME=");

    PLUGINAPI_LOG_INFO("Initialized python runtime");
    return true;
}

void PythonRuntime::Shutdown()
{
    PLUGINAPI_LOG_INFO("Shutting down python");
}

void PythonRuntime::LoadSourceFile()
{
    auto module = pybind11::module::import("test");
}

void PythonRuntime::OnTick()
{
}

void PythonRuntime::BindAction(const PluginAPI::StringRef acActionName, const ArgType* args, size_t argCount,
                               void (*aCallback)(ActionStack& acContext))
{
    
}

void PythonRuntime::InvokeAction(const StringRef acActionName, ActionStack& acStack)
{
}

void PythonRuntime::ExecuteCode(const PluginAPI::StringRef acCode)
{
}

void PythonRuntime::ExecuteFile(const PluginAPI::StringRef acFilePath)
{
    auto [path, filename] = split_filepath(acFilePath.data());

    pybind11::scoped_interpreter guard{};
    AddPath(path.c_str());

    pybind11::module_ pyScript = pybind11::module_::import(filename.c_str());

    const ArgType Types[] = {ArgType::kBool, ArgType::kBool};
    RegisterMethod(*pyScript.ptr(), "test_func", Types, 2, [](ActionStack& acContext) {
        auto a = acContext.PopBool();
        auto b = acContext.PopBool();
        PluginAPI::PluginAPI_WriteLog(PluginAPI::LogLevel::kInfo, "test_func: %d %d", a, b);
    });

    auto res = pyScript.attr("plugin_main")();

    ActionStack stackxx(2);
    stackxx.Push(true);
    stackxx.Push(true);
    CallMethod(*pyScript.ptr(), "test_func", stackxx);
}

} // namespace PythonScripting
