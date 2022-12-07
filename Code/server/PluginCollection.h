#pragma once

#include <filesystem>
#include <vector>

#include "PluginAPI.h"

class PluginCollection
{
  public:
    PluginCollection();
    ~PluginCollection();

    // preload all found plugin modules
    void CollectPlugins(const std::filesystem::path&);

    // component lifetime
    void InitializePlugins();
    void ShutdownPlugins();

    void UnloadPlugins();

    void DumpLoadedPuginsToLog();

    void ForEachPlugin(std::function<void(const PluginDescriptor&)> aCallback) const;

    size_t GetPluginCount() const
    {
        return m_pluginData.size();
    }

  private:
    std::filesystem::path m_pluginPath;

    struct PluginData
    {
        void* pModuleHandle;
        PluginDescriptor* pDescriptor;
        PluginInterface001* pInterface; // < note that this pointer is _NOT_ owned by us, so do not attempt to free it
    };
    std::vector<PluginData> m_pluginData;
};
