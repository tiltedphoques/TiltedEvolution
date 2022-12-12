// Copyright (C) Vincent Hengel 2022

#include "PythonRuntime.h"
#include "Pch.h"
#include "PythonEnvironment.h"
#include "PythonFunction.h"

#include <PluginAPI/Logging.h>

namespace PythonScripting
{
// This is a great resource that i used while embedding python:
// https://pybind11.readthedocs.io/en/stable/advanced/embedding.html
PythonRuntime::~PythonRuntime() noexcept
{
    // clear these predefined env variables so that python doesn't try to use them
    (void)putenv("PYTHONPATH=");
    (void)putenv("PYTHONHOME=");
}

bool PythonRuntime::Initialize()
{
    PLUGINAPI_LOG_INFO("Initialized python runtime");
    return true;
}

void PythonRuntime::Shutdown() { PLUGINAPI_LOG_INFO("Shutting down python"); }

void PythonRuntime::OnTick() {}

PluginInterface001::Handle PythonRuntime::LoadFile(const PluginAPI::StringRef acFilePath)
{
    // for python we have to split the path into the directory and the filename, then add the directory to the python
    // path
    auto [path, filename] = split_filepath(acFilePath.data());
    if (path.empty())
    {
        PLUGINAPI_LOG_ERROR("Path is empty for file %s", acFilePath.data());
        return 0;
    }

    auto& moduleEntry = m_Modules.emplace_back();

    // keep in mind we can only create one real scoped_interpreter.
    try
    {
        // create an interpreter
        moduleEntry.m_pInterpreter = TiltedPhoques::MakeUnique<py::scoped_interpreter>();
    }
    catch (std::exception& ex)
    {
        PLUGINAPI_LOG_ERROR("Failed to create python interpreter: %s", ex.what());
        return 0;
    }

    // must have the interpreter constructed first.
    AddPath(path.c_str());

    try
    {
        moduleEntry.m_Module = std::move(py::module_::import(filename.c_str()));
    }
    catch (std::exception& ex)
    {
        PLUGINAPI_LOG_ERROR("Failed to load python file: %s", ex.what());
        return 0;
    }

    return m_Modules.size();
}

PluginResult PythonRuntime::BindMethod(Handle aHandle, const PluginAPI::StringRef acActionName, const Slice<const ArgType> aArgs, MethodHandler aMethod)
{
    auto& moduleEntry = m_Modules[aHandle - 1];

    if (!PythonScripting::RegisterMethod(*moduleEntry.m_Module.ptr(), acActionName, aArgs.data(), aArgs.size(), aMethod))
    {
        PLUGINAPI_LOG_ERROR("Failed to bind action: %s", acActionName.data());
        return PluginResult::kUnknownError;
    }

    return PluginResult::kOk;
}

PluginResult PythonRuntime::CallMethod(Handle aHandle, const StringRef acActionName, ActionStack& aStack)
{
    auto& moduleEntry = m_Modules[aHandle - 1];

    if (!PythonScripting::CallMethod(*moduleEntry.m_Module.ptr(), acActionName, aStack))
    {
        PLUGINAPI_LOG_ERROR("Failed to invoke action: %s", acActionName.data());
        return PluginResult::kCallFailed;
    }

    return PluginResult::kOk;
}

} // namespace PythonScripting
