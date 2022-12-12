// Copyright (C) Vincent Hengel 2022
#pragma once

#include "Pch.h"
#include <server/PluginAPI/PluginAPI.h>

namespace PythonScripting
{
bool RegisterMethod(PyObject& aModule, const PluginAPI::StringRef acActionName, const PluginAPI::ArgType* apArgs, size_t aArgCount, void (*aCallback)(PluginAPI::ActionStack& acContext));

bool CallMethod(PyObject& aModule, const PluginAPI::StringRef acName, PluginAPI::ActionStack& aActionStack);
} // namespace PythonScripting
