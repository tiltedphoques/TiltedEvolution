#pragma once

#include <server/PluginAPI/PluginAPI.h>

using namespace PluginAPI;

class RemoteControlPlugin final : public PluginAPI::PluginInterface001
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

    uint32_t OnEvent(uint32_t aEventCode) override;

  private:
    // Inherited via PluginInterface001
    void OnTick() override;
};
