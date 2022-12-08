
#include "PythonRuntime.h"
#include "Pch.h"

#include <PluginAPI/Logging.h>

// This is a great resource that i used while embedding python:
// https://pybind11.readthedocs.io/en/stable/advanced/embedding.html
PythonRuntime::~PythonRuntime() noexcept
{
}

bool PythonRuntime::Initialize()
{

    
    PluginAPI_WriteLog(LogLevel::kInfo, "Initialized python runtime");
    return true;
}

void PythonRuntime::Shutdown()
{
    PluginAPI_WriteLog(LogLevel::kInfo, "Shutting down python");
}

void PythonRuntime::LoadSourceFile()
{
    // py::scoped_interpreter guard{};
    // py::exec(R"(
    //     import sys
    //     print(sys.version)
    //     print("Hello, World!")
    //)");
    auto module = pybind11::module::import("test");
    
}

void PythonRuntime::OnTick()
{
}
