
#include "PythonRuntime.h"
#include "Pch.h"

#include <PluginAPI/Logging.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
PYBIND11_EMBEDDED_MODULE(fast_calc, m)
{
    // `m` is a `py::module_` which is used to bind functions and classes
    m.def("add", [](int i, int j) { return i + j; });
}

void TestPython()
{
    pybind11::scoped_interpreter guard{};
    pybind11::exec(R"(
         import sys
         print(sys.version)
         print("Hello, World!")
    )");
}
}

// This is a great resource that i used while embedding python:
// https://pybind11.readthedocs.io/en/stable/advanced/embedding.html
PythonRuntime::~PythonRuntime() noexcept
{
}

bool PythonRuntime::Initialize()
{
    // configure the current python home path!
    //SetEnvironmentVariableW(LR"");
    // PuUT DlLS, lib, tcl in where SkyrimTogetherServer.exe is

    putenv(R"("PYTHONPATH"=C:\Users\vince\Projects\TiltedEvolution\build\windows\x64\debug\plugins;C:\Users\vince\Projects\TiltedEvolution\build\windows\x64\debug\plugins\Lib;C:\Users\vince\Projects\TiltedEvolution\build\windows\x64\debug\plugins\DLLs)");
    putenv(R"("PYTHONHOME"=C:\Users\vince\Projects\TiltedEvolution\build\windows\x64\debug\plugins)");

    TestPython();
    PluginAPI_WriteLog(LogLevel::kInfo, "Initialized python runtime");
    return true;
}

void PythonRuntime::Shutdown()
{
    PluginAPI_WriteLog(LogLevel::kInfo, "Shutting down python");
}

void PythonRuntime::LoadSourceFile()
{
    auto module = pybind11::module::import("test");
    
}

void PythonRuntime::OnTick()
{
}
