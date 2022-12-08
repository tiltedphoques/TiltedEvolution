#pragma once

#include <server/PluginAPI/Action.h>
#include <server/PluginAPI/Logging.h>
#include <server/PluginAPI/PluginAPI.h>

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

    void OnEvent(uint32_t aEventCode) override;

  private:
    // Inherited via PluginInterface001
    void OnTick() override;
};
