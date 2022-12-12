// Copyright (C) Vincent Hengel 2022

#include "PythonFunction.h"
#include <PluginAPI/Logging.h>
#include <PluginAPI/PluginAPI.h>

namespace PythonScripting
{
using namespace PluginAPI;

namespace
{
struct ArgList final : PyObject
{
    explicit ArgList(const ArgType* apTypeList, const size_t aArgCount, void (*apFunctor)(ActionStack&))
        : pTypes(apTypeList)
        , count(aArgCount)
        , functor(apFunctor)
    {
        PyObject::ob_refcnt = 0;
        PyObject::ob_type = &PyType_Type;
    }

    const ArgType* pTypes;
    size_t count;
    void (*functor)(ActionStack&);
};

PyObject* PythonMethodHandler(PyObject* apSelf, PyObject* const* apArgs, Py_ssize_t aArgNumber, PyObject* apKeywords)
{
    // cast self back to full type.
    const ArgList* pTypeInfo = reinterpret_cast<const ArgList*>(apSelf);

    // no arguments, bail out.
    if (pTypeInfo->count == 0)
    {
        ActionStack stack(0);
        stack.Finish();
        pTypeInfo->functor(stack);
        Py_RETURN_NONE;
    }

    // https://github.com/python/cpython/blob/d5f8a2b6ad408368e728a389da918cead3ef7ee9/Python/getargs.c#L655
    ActionStack stack(pTypeInfo->count);

    for (auto i = 0; i < aArgNumber; i++)
    {
        auto* pObject = const_cast<PyObject*>(apArgs[i]);
        switch (pTypeInfo->pTypes[i])
        {
        default:
        {
            PLUGINAPI_LOG_ERROR("Unknown type info for type at index: %d", i);
            break;
        }
        case ArgType::kBool:
        {
            auto result = PyObject_IsTrue(pObject);
            if (result == -1)
            {
                PLUGINAPI_LOG_ERROR("Expected a boolean");
                return nullptr;
            }
            stack.Push(result == 1);
            break;
        }
        case ArgType::kU32:
        {
            uint32_t value = PyLong_AsUnsignedLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                PLUGINAPI_LOG_ERROR("Expected an unsigned int");
                return nullptr;
            }
            stack.Push(value);
            break;
        }
        case ArgType::KI32:
        {
            int32_t value = _PyLong_AsInt(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                PLUGINAPI_LOG_ERROR("Expected an int");
                return nullptr;
            }
            stack.Push(value);
            break;
        }
        case ArgType::kU64:
        {
            uint64_t value = PyLong_AsUnsignedLongLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                PLUGINAPI_LOG_ERROR("Expected an unsigned long long");
                return nullptr;
            }
            stack.Push(value);
            break;
        }
        case ArgType::kI64:
        {
            int64_t value = PyLong_AsLongLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                PLUGINAPI_LOG_ERROR("Expected an long long");
                return nullptr;
            }

            stack.Push(value);
            break;
        }
        case ArgType::kF32:
        {
            double value = PyFloat_AsDouble(pObject);
            if (value == -1.0 && PyErr_Occurred())
            {
                PLUGINAPI_LOG_ERROR("Expected a float");
                return nullptr;
            }

            stack.Push(static_cast<float>(value));
            break;
        }
        case ArgType::kString:
        {
            if (!PyUnicode_Check(pObject))
            {
                PLUGINAPI_LOG_ERROR("Expected a string");
                return nullptr;
            }

            auto* pString = PyUnicode_AsUTF8(pObject);
            stack.PushString(PluginAPI::StringRef(pString, std::strlen(pString)));
            break;
        }
        }
    }

    stack.Finish();
    pTypeInfo->functor(stack);
    // Return None
    Py_RETURN_NONE;
}
} // namespace

bool RegisterMethod(PyObject& aModule, const PluginAPI::StringRef acActionName, const ArgType* apArgs, size_t aArgCount, void (*aCallback)(ActionStack& acContext))
{
    PyMethodDef* pDef = new PyMethodDef{.ml_name = acActionName.data(), .ml_meth = reinterpret_cast<PyCFunction>(PythonMethodHandler), .ml_flags = METH_FASTCALL, .ml_doc = nullptr};

    auto* pSelfObject = reinterpret_cast<PyObject*>((new ArgList(&apArgs[0], aArgCount, aCallback)));
    Py_INCREF(pSelfObject);

    PyObject* pFunction = PyCFunction_New(pDef, pSelfObject);
    if (PyModule_AddObject(&aModule, acActionName.data(), pFunction) < 0)
    {
        Py_DECREF(&pFunction);
        return false;
    }
    return true;
}

// https://github.com/python/cpython/blob/2e279e85fece187b6058718ac7e82d1692461e26/Objects/call.c
// https://github.com/python/cpython/blob/2e279e85fece187b6058718ac7e82d1692461e26/Objects/call.c#L898
bool CallMethod(PyObject& aModule, const StringRef acName, ActionStack& aActionStack)
{
    PyObject* pFunc = PyObject_GetAttrString(&aModule, acName.data());
    if (!pFunc)
    {
        PyErr_Print();
        return false;
    }

    aActionStack.Finish();
    const auto argCount = aActionStack.count();

    // create a py stack.
    PyObject** stack;

    PyObject* small_stack[_PY_FASTCALL_SMALL_STACK]{};
    if (argCount <= (Py_ssize_t)Py_ARRAY_LENGTH(small_stack))
        stack = small_stack;
    else
    {
        stack = reinterpret_cast<PyObject**>(PyMem_Malloc(argCount * sizeof(stack[0])));
        if (stack == NULL)
        {
            PyErr_NoMemory();
            return false;
        }
    }
    // set base object for method.
    int stackCounter = 0;
    stack[stackCounter++] = pFunc;

    for (uint32_t i = 0; i < argCount; i++)
    {
        auto type = aActionStack.GetArgType(i);
        switch (type)
        {
        case ArgType::kUnknown:
        {
            if (stack != small_stack)
            {
                PyMem_Free(stack);
            }

            PLUGINAPI_LOG_ERROR("Unknown type at index: %d (Out of bounds access)", i);
            return false;
        }
        case ArgType::kBool:
        {
            auto v = aActionStack.PopBool();
            stack[stackCounter++] = PyBool_FromLong(static_cast<long>(v));
            break;
        }
        case ArgType::kU32:
        {
            uint32_t v = aActionStack.PopU32();
            stack[stackCounter++] = PyLong_FromUnsignedLong(v);
            break;
        }
        case ArgType::KI32:
        {
            int32_t v = aActionStack.PopI32();
            stack[stackCounter++] = PyLong_FromLong(v);
            break;
        }
        case ArgType::kU64:
        {
            uint64_t v = aActionStack.PopU64();
            stack[stackCounter++] = PyLong_FromUnsignedLongLong(v);
            break;
        }
        case ArgType::kI64:
        {
            int64_t v = aActionStack.PopU64();
            stack[stackCounter++] = PyLong_FromLongLong(v);
            break;
        }
        case ArgType::kF32:
        {
            float v = aActionStack.PopF32();
            stack[stackCounter++] = PyFloat_FromDouble(v);
            break;
        }
        case ArgType::kString:
        {
            TiltedPhoques::String string;
            aActionStack.PopString(string);
            stack[stackCounter++] = PyUnicode_FromString(string.c_str());
            break;
        }
        }
    }

    PyThreadState* tstate = PyThreadState_Get();
    PyObject* pResult = _PyObject_VectorcallTstate(tstate, pFunc, stack, argCount, NULL);
    if (pResult)
    {
    }

    if (stack != small_stack)
    {
        PyMem_Free(stack);
    }
    return true;
}
} // namespace PythonScripting
