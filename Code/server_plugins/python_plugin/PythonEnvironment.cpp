// Copyright (C) Vincent Hengel 2022

#include "Pch.h"

namespace PythonScripting
{
bool AddPath(const char* acPath)
{
    // https://stackoverflow.com/a/46465617
    std::string path = acPath;
    std::replace(path.begin(), path.end(), '\\', '/');

    PyObject* sys = PyImport_ImportModule("sys");
    PyObject* sys_path = PyObject_GetAttrString(sys, "path");
    PyObject* folder_path = PyUnicode_FromString(path.c_str());
    int rval = PyList_Append(sys_path, folder_path);

    Py_DECREF(folder_path);

    return rval == 0;
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
}
