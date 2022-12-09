#pragma once

#include <filesystem>
#include <vector>

#include "PluginAPI/PluginAPI.h"

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

    void ForEachPlugin(std::function<void(const PluginDescriptor&, IPluginInterface&)> aCallback) const;

    size_t GetPluginCount() const
    {
        return m_pluginData.size();
    }

  private:
    bool TryLoadPlugin(const std::filesystem::path& aPath);

  private:
    std::filesystem::path m_pluginPath;

    struct PluginData
    {
        void* pModuleHandle;
        const PluginDescriptor* pDescriptor;
        IPluginInterface* pInterface; // < note that this pointer is _NOT_ owned by us, so do not attempt to free it
    };
    std::vector<PluginData> m_pluginData;
};
