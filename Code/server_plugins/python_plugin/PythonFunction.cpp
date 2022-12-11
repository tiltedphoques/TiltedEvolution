// Copyright (C) Vincent Hengel 2022

#include "PythonFunction.h"
#include <PluginAPI/PluginAPI.h>

namespace PythonScripting
{
using namespace PluginAPI;

namespace
{
struct ArgList final : PyObject
{
    explicit ArgList(const ArgType* apTypeList, const size_t aArgCount, void (*apFunctor)(ActionStack&))
        : pTypes(apTypeList), count(aArgCount), functor(apFunctor)
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
    // we pass our own metadata as self parameter.
    const ArgList* b = reinterpret_cast<const ArgList*>(apSelf);
    if (b->count == 0)
    {
        ActionStack stack(0);
        stack.End();
        b->functor(stack);
        Py_RETURN_NONE;
    }

    // https://github.com/python/cpython/blob/d5f8a2b6ad408368e728a389da918cead3ef7ee9/Python/getargs.c#L655
    ActionStack stack(b->count);
    for (auto i = 0; i < aArgNumber; i++)
    {
        auto* pObject = (PyObject*)(apArgs[i]);
        switch (b->pTypes[i])
        {
        case ArgType::kBool: {
            auto result = PyObject_IsTrue(pObject);
            if (result == -1)
            {
                PyErr_SetString(PyExc_TypeError, "Expected a boolean");
                return nullptr;
            }

            stack.Push(result == 1);
            break;
        }
        case ArgType::kU8: {
            int value = PyLong_AsLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                PyErr_Print();
                PyErr_Clear();
                continue;
            }
            else
                stack.Push(static_cast<uint8_t>(value));
            break;
        }
        case ArgType::kI8: {
            int value = PyLong_AsLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                // Handle error
            }
            else
            {
                stack.Push(static_cast<int8_t>(value));
            }
            break;
        }
        case ArgType::kU16: {
            int value = PyLong_AsLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                // Handle error
            }
            else
            {
                stack.Push(static_cast<uint16_t>(value));
            }
            break;
        }
        case ArgType::kI16: {
            int value = PyLong_AsLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                // Handle error
            }
            else
            {
                stack.Push(static_cast<int16_t>(value));
            }
            break;
        }
        case ArgType::kU32: {
            long value = PyLong_AsLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                // Handle error
            }
            else
            {
                stack.Push(static_cast<uint32_t>(value));
            }
            break;
        }
        case ArgType::KI32: {
            long value = PyLong_AsLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                // Handle error
            }
            else
            {
                stack.Push(static_cast<int32_t>(value));
            }
            break;
        }
        case ArgType::kU64: {
            PY_LONG_LONG value = PyLong_AsLongLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                // Handle error
            }
            else
            {
                stack.Push(static_cast<uint64_t>(value));
            }
            break;
        }
        case ArgType::kI64: {
            PY_LONG_LONG value = PyLong_AsLongLong(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                // Handle error
            }
            else
            {
                stack.Push(static_cast<int64_t>(value));
            }
            break;
        }
        case ArgType::kF32: {
            double value = PyFloat_AsDouble(pObject);
            if (value == -1 && PyErr_Occurred())
            {
                // Handle error
            }
        }
        }
    }

    stack.End();
    b->functor(stack);
    // Return None
    Py_RETURN_NONE;
}
} // namespace

bool RegisterMethod(PyObject& aModule, const PluginAPI::StringRef acActionName, const ArgType* apArgs, size_t aArgCount,
                    void (*aCallback)(ActionStack& acContext))
{
    PyMethodDef* pDef = new PyMethodDef{.ml_name = acActionName.data(),
                                        .ml_meth = reinterpret_cast<PyCFunction>(PythonMethodHandler),
                                        .ml_flags = METH_FASTCALL,
                                        .ml_doc = nullptr};

    auto *pSelfObject = reinterpret_cast<PyObject*>((new ArgList(&apArgs[0], aArgCount, aCallback)));
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

    aActionStack.End();
    const auto argCount = aActionStack.count();

    // create a py stack.
    PyObject** stack;

    PyObject* small_stack[_PY_FASTCALL_SMALL_STACK]{};
    if (argCount <= (Py_ssize_t)Py_ARRAY_LENGTH(small_stack))
        stack = small_stack;
    else
    {
        stack = (PyObject**)PyMem_Malloc(argCount * sizeof(stack[0]));
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
        auto type = aActionStack.GetArg(i);
        switch (type)
        {
        case ArgType::kBool: {
            auto val = aActionStack.PopBool();
            stack[stackCounter++] = PyBool_FromLong(static_cast<long>(val));
            break;
        }
        }
    }

    PyThreadState* tstate = PyThreadState_Get();
    PyObject* result = _PyObject_VectorcallTstate(tstate, pFunc, stack, argCount, NULL);

    if (stack != small_stack)
    {
        PyMem_Free(stack);
    }
    return true;
}
} // namespace PythonScripting
