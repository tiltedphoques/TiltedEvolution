// Copyright (C) Vincent Hengel 2022
#pragma once

#include "Pch.h"
#include <server/PluginAPI/PluginAPI.h>

namespace PythonScripting
{
using namespace PluginAPI;

class PythonRuntime final : public PluginInterface001
{
public:
    ~PythonRuntime() noexcept override;

    // Inherited via PluginInterface001
    uint32_t GetVersion() override { return 1; }

    bool Initialize() override;
    void Shutdown() override;

private:
    // Inherited via PluginInterface001
    void OnTick() override;

    // this method is not thread safe, please call in order.
    Handle LoadFile(const StringRef acFileName) override;

    PluginResult BindMethod(Handle aHandle, const StringRef acActionName, const Slice<const ArgType> aArgs, MethodHandler aMethod) override;
    PluginResult CallMethod(Handle aHandle, const StringRef acActionName, ActionStack& acStack) override;

private:
    bool m_bInitialized = false;
    Handle m_NextHandle = 0;
    struct PythonModule
    {
        Handle m_Handle;
        py::module_ m_Module; // the entry point module
    };
    TiltedPhoques::UniquePtr<TiltedPhoques::Vector<PythonModule>> m_Modules;
};

} // namespace PythonScripting
