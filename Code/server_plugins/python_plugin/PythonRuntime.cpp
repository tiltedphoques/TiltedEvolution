
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
} // namespace

// This is a great resource that i used while embedding python:
// https://pybind11.readthedocs.io/en/stable/advanced/embedding.html
PythonRuntime::~PythonRuntime() noexcept
{
}

bool PythonRuntime::Initialize()
{
    // configure the current python home path!
    // SetEnvironmentVariableW(LR"");
    // PuUT DlLS, lib, tcl in where SkyrimTogetherServer.exe is

    putenv(
        R"("PYTHONPATH"=C:\Users\vince\Projects\TiltedEvolution\build\windows\x64\debug\plugins;C:\Users\vince\Projects\TiltedEvolution\build\windows\x64\debug\plugins\Lib;C:\Users\vince\Projects\TiltedEvolution\build\windows\x64\debug\plugins\DLLs)");
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

void PythonRuntime::ExecuteCode(const PluginSlice<char> acCode)
{
}

PyObject* loadModule(const char* modulePath, const char* moduleName)
{
    auto modules = PyImport_GetModuleDict();
    auto impModule = PyDict_GetItemString(modules, "imp");
    if (impModule)
    {
        // GetItemString returns a borrowed reference, but ImportModule
        // returns a new reference, so INCREF here to even it out
        Py_INCREF(impModule);
    }
    else
    {
        impModule = PyImport_ImportModule("imp");
        if (!impModule)
        {
            // we've tried hard enough, bail out
            PyErr_Print();
            return nullptr;
        }
    }

    // The Python API asks for non-const char pointers :(
    char methodName[] = "load_source";
    char args[] = "ss";
    auto module = PyObject_CallMethod(impModule, methodName, args, moduleName, modulePath);

    Py_XDECREF(impModule);
    Py_XDECREF(modulePath);
    return module;
}

void AddPath(const char* acPath)
{
    // https://stackoverflow.com/a/46465617
    std::string path = acPath;
    std::replace(path.begin(), path.end(), '\\', '/');

    PyObject* sys = PyImport_ImportModule("sys");
    PyObject* sys_path = PyObject_GetAttrString(sys, "path");
    PyObject* folder_path = PyUnicode_FromString(path.c_str());
    PyList_Append(sys_path, folder_path);
}

std::pair<std::string, std::string> split_filepath(const std::string& filepath)
{
    size_t pos = filepath.find_last_of('/');
    if (pos == std::string::npos)
        pos = filepath.find_last_of('\\');

    if (pos == std::string::npos)
        return {"", filepath};

    // we also strip the extension at the end of the filename for python..
    return {filepath.substr(0, pos), filepath.substr(pos + 1, filepath.find_last_of('.') - (pos + 1))};
}

void PythonRuntime::ExecuteFile(const PluginSlice<char> acFilePath)
{
    auto [path, filename] = split_filepath(acFilePath.data());

    pybind11::scoped_interpreter guard{};
    AddPath(path.c_str());

    pybind11::module_ pyScript = pybind11::module_::import(filename.c_str());

    auto result = pyScript.attr("plugin_main")();

    //
    // auto* pObject = loadModule(acFileName.data(), "test_script");

    // auto module = pybind11::module::import(acFileName.data());
}
