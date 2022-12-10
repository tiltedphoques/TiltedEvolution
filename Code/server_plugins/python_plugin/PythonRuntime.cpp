
#include "PythonRuntime.h"
#include "Pch.h"

#include <PluginAPI/Logging.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace py = pybind11;

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

py::tuple ActionStackToArgs(ActionStack& stack)
{
    std::vector<py::object> args;

    for (uint32_t i = 0; i < stack.count(); ++i)
    {
        ArgType type = stack.GetArg(i);

        switch (type)
        {
        case ArgType::kBool:
            args.push_back(py::bool_(stack.PopBool()));
            break;
        case ArgType::kF32:
            args.push_back(py::float_(stack.PopF32()));
            break;
        case ArgType::kU8:
            args.push_back(py::int_(stack.PopU8()));
            break;
        case ArgType::kU16:
            args.push_back(py::int_(stack.PopU16()));
            break;
        case ArgType::kU32:
            args.push_back(py::int_(stack.PopU32()));
            break;
        case ArgType::kU64:
            args.push_back(py::int_(stack.PopU64()));
            break;
        case ArgType::kI8:
            args.push_back(py::int_(stack.PopI8()));
            break;
        case ArgType::kI16:
            args.push_back(py::int_(stack.PopI16()));
            break;
        case ArgType::KI32:
            args.push_back(py::int_(stack.PopI32()));
            break;
        case ArgType::kI64:
            args.push_back(py::int_(stack.PopI64()));
            break;
        }
    }
    return py::make_tuple(args);
}

ActionStack ArgsToActionStack(const py::tuple& args)
{
    // Create an ActionStack with the same number of arguments as the tuple
    ActionStack stack(args.size());

    // Loop through the arguments in the tuple
    for (uint32_t i = 0; i < args.size(); ++i)
    {
        // Get the type of the current argument
        const auto& arg = args[i];
        const auto type = arg.get_type();

        // Convert the argument to the appropriate type and push it onto the stack
        if (py::isinstance(type, py::bool_()))
            stack.Push(arg.cast<bool>());
        else if (py::isinstance(type, py::float_()))
            stack.Push(arg.cast<float>());
        else if (py::isinstance(type, py::int_()))
            stack.Push(arg.cast<int64_t>());
    }

    // Return the ActionStack
    return stack;
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
    PluginAPI::PluginAPI_WriteLog(PluginAPI::LogLevel::kInfo, "Initialized python runtime");
    return true;
}

void PythonRuntime::Shutdown()
{
    PluginAPI::PluginAPI_WriteLog(PluginAPI::LogLevel::kInfo, "Shutting down python");
}

void PythonRuntime::LoadSourceFile()
{
    auto module = pybind11::module::import("test");
}

void PythonRuntime::OnTick()
{
}
#if 0
ActionStack ConvertToActionStack(const pybind11::args& args)
{
    // Create an ActionStack instance with the same number of arguments as the input pybind11::args object
    ActionStack stack(args.size());

    // Iterate over the pybind11::args object and push each argument to the ActionStack instance
    for (const auto& arg : args)
    {
        if (arg.is_bool())
        {
            stack.Push(arg.cast<bool>());
        }
        else if (arg.is_int())
        {
            // Depending on the size of the integer, you can push it to the ActionStack as a signed or unsigned 8-bit,
            // 16-bit, 32-bit, or 64-bit value
            if (arg.cast<int>() <= std::numeric_limits<int8_t>::max())
            {
                stack.Push(arg.cast<int8_t>());
            }
            else if (arg.cast<int>() <= std::numeric_limits<int16_t>::max())
            {
                stack.Push(arg.cast<int16_t>());
            }
            else if (arg.cast<int>() <= std::numeric_limits<int32_t>::max())
            {
                stack.Push(arg.cast<int32_t>());
            }
            else
            {
                stack.Push(arg.cast<int64_t>());
            }
        }
        else if (arg.is_float())
        {
            // You can push a floating-point value to the ActionStack as a 32-bit floating-point value
            stack.Push(arg.cast<float>());
        }
        else if (arg.is_string())
        {
            stack.Push(arg.cast<std::string>());
        }
        // Handle other argument types as needed...
    }

    return stack;
}
#endif

void PythonRuntime::BindAction(const PluginAPI::StringRef acActionName, const ArgType* args, size_t argCount,
                               void (*aCallback)(ActionStack& acContext))

{
    return;
    pybind11::module_ pyScript;
    pyScript.def(acActionName.data(), [&](const py::args& acContext) {
        auto stack = ArgsToActionStack(acContext);
        return aCallback(stack);
    });
}

void PythonRuntime::InvokeAction(const StringRef acActionName, ActionStack& acStack)
{
    pybind11::module_ pyScript;
    pyScript.attr(acActionName.data())(ActionStackToArgs(acStack));
}

void PythonRuntime::ExecuteCode(const PluginAPI::StringRef acCode)
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

struct ArgList : PyObject
{
    ArgList(const ArgType* at, const size_t count, void (*f)(ActionStack&)) : pTypes(at), count(count), functor(f)
    {
        PyObject::ob_refcnt = 0;
        PyObject::ob_type = &PyType_Type;
    }

    const ArgType* pTypes;
    size_t count;
    void (*functor)(ActionStack&);
};

#define _Py_RVALUE(EXPR) ((void)0, (EXPR))
#define _PyTuple_ITEMS(op) _Py_RVALUE(_PyTuple_CAST(op)->ob_item)

// PyArg_Parse(PyObject *args, const char *format, ...)
// -> retval = vgetargs1(args, format, &va, FLAG_COMPAT);
// -> vgetargs1(PyObject *args, const char *format, va_list *p_va, int flags)
// -> vgetargs1_impl(PyObject *compat_args, PyObject *const *stack, Py_ssize_t nargs, const char *format, va_list *p_va,
// int flags)
//
static PyObject* my_method(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* keywords)
{
    // we pass our own metadata as self parameter.
    const ArgList* b = reinterpret_cast<const ArgList*>(self);
    if (b->count == 0)
    {
        ActionStack stack(0);
        stack.End();
        b->functor(stack);
        Py_RETURN_NONE;
    }

    // https://github.com/python/cpython/blob/d5f8a2b6ad408368e728a389da918cead3ef7ee9/Python/getargs.c#L655
    ActionStack stack(b->count);
    for (size_t i = 0; i < nargs; i++)
    {
        auto* pObject = (PyObject*)(args[i]);
        switch (b->pTypes[i])
        {
        case ArgType::kBool:
            stack.Push(PyObject_IsTrue(pObject) > 0 ? true : false);
            break;
        }
    }

    stack.End();
    b->functor(stack);
    // Return None
    Py_RETURN_NONE;
}

void RegisterLambda(PyObject* module, const PluginAPI::StringRef acActionName, const ArgType* args, size_t argCount,
                    void (*aCallback)(ActionStack& acContext))
{
    PyMethodDef* pDef = new PyMethodDef{
        .ml_name = "test_func", .ml_meth = (PyCFunction)(my_method), .ml_flags = METH_FASTCALL, .ml_doc = nullptr};

    auto fakeobj = (PyObject*)(new ArgList(&args[0], argCount, aCallback));
    Py_INCREF(fakeobj);

    PyObject* pFunction = PyCFunction_New(pDef, fakeobj);

    if (PyModule_AddObject(module, "test_func", pFunction) < 0)
    {
        //  Py_DECREF(&pFunction);
    }
}

#include <methodobject.h>

void PythonRuntime::ExecuteFile(const PluginAPI::StringRef acFilePath)
{
    auto [path, filename] = split_filepath(acFilePath.data());

    pybind11::scoped_interpreter guard{};
    AddPath(path.c_str());

    pybind11::module_ pyScript = pybind11::module_::import(filename.c_str());

    const ArgType Types[] = {ArgType::kBool, ArgType::kBool};
    RegisterLambda(pyScript.ptr(), "test_func", Types, 2, [](ActionStack& acContext) {
        auto a = acContext.PopBool();
        auto b = acContext.PopBool();
        PluginAPI::PluginAPI_WriteLog(PluginAPI::LogLevel::kInfo, "test_func: %d %d", a, b);
    });

    auto res = pyScript.attr("plugin_main")();
}
