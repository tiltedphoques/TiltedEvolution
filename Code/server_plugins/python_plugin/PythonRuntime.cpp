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
    try
    {
        // try to create the global main interpreter.
        py::initialize_interpreter();

        m_Modules = TiltedPhoques::MakeUnique<TiltedPhoques::Vector<PythonModule>>();
    }
    catch (std::exception& ex)
    {
        PLUGINAPI_LOG_ERROR("Failed to create python interpreter: %s", ex.what());
        return false;
    }

    m_bInitialized = true;
    PLUGINAPI_LOG_INFO("Initialized python runtime");
    return true;
}

void PythonRuntime::Shutdown()
{
    if (m_bInitialized)
    {
        PLUGINAPI_LOG_INFO("Shutting down python");

        // free bound resources first.
        m_Modules.reset(nullptr);

        py::finalize_interpreter();

        PLUGINAPI_LOG_INFO("Sucessfully shut down python");
    }

    if (m_NextHandle > 0)
        m_NextHandle = 0;
}

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

    auto& moduleEntry = m_Modules->emplace_back();

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

    // we have 65k "slots" before we overflow, and that is per runtime, so the universe dies before we load 65k modules
    moduleEntry.m_Handle = m_NextHandle;
    m_NextHandle++;

    return moduleEntry.m_Handle;
}

PluginResult PythonRuntime::BindMethod(Handle aHandle, const PluginAPI::StringRef acActionName, const Slice<const ArgType> aArgs, MethodHandler aMethod)
{
    auto& moduleEntry = (*m_Modules)[aHandle - 1];

    if (!PythonScripting::RegisterMethod(*moduleEntry.m_Module.ptr(), acActionName, aArgs.data(), aArgs.size(), aMethod))
    {
        PLUGINAPI_LOG_ERROR("Failed to bind action: %s", acActionName.data());
        return PluginResult::kUnknownError;
    }

    return PluginResult::kOk;
}

PluginResult PythonRuntime::CallMethod(Handle aHandle, const StringRef acActionName, ActionStack& aStack)
{
    auto& moduleEntry = (*m_Modules)[aHandle - 1];

    if (!PythonScripting::CallMethod(*moduleEntry.m_Module.ptr(), acActionName, aStack))
    {
        PLUGINAPI_LOG_ERROR("Failed to invoke action: %s", acActionName.data());
        return PluginResult::kCallFailed;
    }

    return PluginResult::kOk;
}

} // namespace PythonScripting
