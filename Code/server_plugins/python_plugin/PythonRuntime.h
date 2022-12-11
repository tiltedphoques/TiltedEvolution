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
    uint32_t GetVersion() override
    {
        return 1;
    }

    bool Initialize() override;
    void Shutdown() override;

  private:
    std::unique_ptr<pybind11::scoped_interpreter> pz;

    // Inherited via PluginInterface001
    void OnTick() override;

    // this method is not thread safe, please call in order.
    Handle LoadFile(const StringRef acFileName) override;

    PluginResult BindMethod(Handle aHandle, const StringRef acActionName, const ArgType* apArgs, size_t aArgCount,
                    void (*apCallback)(ActionStack& acContext)) override;
    PluginResult CallMethod(Handle aHandle, const StringRef acActionName, ActionStack& acStack) override;

    struct PythonModule
    {
        TiltedPhoques::UniquePtr<py::scoped_interpreter> m_pInterpreter;
        py::module_ m_Module; // the entry point module
    };
    TiltedPhoques::Vector<PythonModule> m_Modules;
};

} // namespace PythonScripting
