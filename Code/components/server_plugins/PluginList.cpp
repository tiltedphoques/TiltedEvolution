// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <Windows.h>
#include <server_plugins/PluginList.h>

namespace server_plugins
{
namespace
{
} // namespace

namespace fs = std::filesystem;

static constexpr char kPluginFolderName[] = "plugins";
static constexpr char kNativePluginExtension[] = ".dll";

PluginList::~PluginList()
{
    DumpPlugins();
}

void PluginList::RefreshList(const fs::path& acPath)
{
    m_pluginPath = acPath / kPluginFolderName;

    std::vector<fs::path> canidates;
    for (auto const& dir : std::filesystem::directory_iterator{m_pluginPath})
    {
        if (dir.path().extension() == kNativePluginExtension)
        {
            canidates.push_back(dir.path());
        }
    }

    for (const auto& path : canidates)
    {
        if (auto pHandle = LoadLibraryW(path.c_str()))
        {
            if (PluginDescriptor* pluginDescriptor =
                    reinterpret_cast<PluginDescriptor*>(GetProcAddress(pHandle, "plugin")))
            {
                m_pluginData.emplace_back(PluginData::Type::kRuntimePlugin, static_cast<void*>(pHandle), pluginDescriptor);
            }
        }
    }
}

void PluginList::DumpPlugins()
{
    for (const PluginData& cData : m_pluginData)
    {
        FreeLibrary(static_cast<HMODULE>(cData.m_pModuleHandle));
    }
}
} // namespace server_plugins
