// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <filesystem>
#include <vector>
#include <server_plugins/Plugin.h>

namespace server_plugins
{
// PluginList/Registry/Index/Table?
class PluginList
{
  public:
    ~PluginList();

    void RefreshList(const std::filesystem::path&);
    void DumpPlugins();

    auto& GetList()
    {
        return m_pluginData;
    }

  private:
    std::filesystem::path m_pluginPath;
    std::vector<PluginData> m_pluginData;
};
} // namespace server_plugins
