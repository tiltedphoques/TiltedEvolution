#pragma once

#include "Pch.h"
#include <server/PluginAPI/PluginAPI.h>

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

    void LoadSourceFile();
  private:
    std::unique_ptr<pybind11::scoped_interpreter> pz;

    // Inherited via PluginInterface001
    virtual void OnTick() override;
    // TiltedPhoques::UniquePointer<py::scoped_interpreter> m_guard;
};
