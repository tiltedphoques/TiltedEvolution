#pragma once

#include <server/PluginAPI.h>

class RemoteControlPlugin final : public PluginInterface001
{
  public:
    ~RemoteControlPlugin() override;

    // Inherited via PluginInterface001
    uint32_t GetVersion() override
    {
        // we subscribe to the v1 api
        return 1;
    }

    bool Initialize() override;
    void Shutdown() override;

    void CallScriptFunction(const PluginStringView aName, ScriptFunctionContext&);
    void BindScriptFunction(const PluginStringView aName, void* apFunctor, const ArgType* apArgs,
                            const size_t aArgCount);

  private:
};
