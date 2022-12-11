// Copyright (C) Vincent Hengel 2022
#pragma once

#include "Pch.h"
#include <server/PluginAPI/PluginAPI.h>

namespace PythonScripting
{
using namespace PluginAPI;

class PythonRuntime final : public PluginAPI::PluginInterface001
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

    void LoadSourceFile();

  private:
    std::unique_ptr<pybind11::scoped_interpreter> pz;

    // Inherited via PluginInterface001
    void OnTick() override;

    void BindAction(const StringRef acActionName, const ArgType* args, size_t argCount,
                    void (*aCallback)(ActionStack& acContext)) override;
    void InvokeAction(const StringRef acActionName, ActionStack& acStack) override;
    void ExecuteCode(const StringRef acCode) override;
    
    void ExecuteFile(const StringRef acFileName) override;
    // TiltedPhoques::UniquePointer<py::scoped_interpreter> m_guard;
};

} // namespace PythonScripting
