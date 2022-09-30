#pragma once

#include <server/PluginAPI.h>

class LuaRuntime final : public PluginInterface001
{
  public:
    ~LuaRuntime() override = default;

    // Inherited via PluginInterface001
    virtual bool Initialize() override;
    virtual void Shutdown() override;

    // Inherited via PluginInterface001
    virtual uint32_t GetVersion() override;
};
