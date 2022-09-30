
#include "PluginCollection.h"
#include "Pch.h"

static constexpr char kPluginFolderName[] = "plugins";
static constexpr char kNativePluginExtension[] = ".dll";

PluginCollection::PluginCollection()
{
    CollectPlugins(std::filesystem::current_path());
}

PluginCollection::~PluginCollection()
{
    UnloadPlugins();
}

void PluginCollection::CollectPlugins(const std::filesystem::path& acPath)
{
    m_pluginPath = acPath / kPluginFolderName;

    std::vector<std::filesystem::path> canidates;
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
            if (PluginDescriptor* pPluginDescriptor =
                    reinterpret_cast<PluginDescriptor*>(GetProcAddress(pHandle, "TT_PLUGIN")))
            {
                // TODO: parse out version bits
                if (pPluginDescriptor->magic == kPluginMagic)
                    m_pluginData.emplace_back(pHandle, pPluginDescriptor, nullptr);
                else
                    spdlog::error("Invalid plugin magic in descriptor");
            }
        }
    }
}

void PluginCollection::InitializePlugins()
{
    for (PluginData& data : m_pluginData)
    {
        if (!data.pDescriptor)
        {
            spdlog::error("Unable to fetch descriptor for collected plugin.");
            continue;
        }

        // TODO: do someting with the reurned interface version.
        PluginInterface001* pInstance = data.pDescriptor->CreatePlugin();
        if (!pInstance)
        {
            spdlog::error(
                "Descriptor->CreatePlugin() for {} returned null. Did you forget to allocate the plugin instance?",
                data.pDescriptor->pName);
            continue;
        }

        data.pInterface = pInstance;

        bool result = pInstance->Initialize();
        if (!result)
        {
            spdlog::error("plugin->Initialize() for {} returned false. Plugin initialization failed.",
                          data.pDescriptor->pName);

            data.pInterface = nullptr;
            continue;
        }
    }
}

void PluginCollection::ShutdownPlugins()
{
    for (PluginData& data : m_pluginData)
    {
        if (!data.pDescriptor)
        {
            spdlog::error("Unable to fetch descriptor for collected plugin.");
            continue;
        }

        if (!data.pInterface)
        {
            spdlog::error("Unable to fetch interface for plugin {}.", data.pDescriptor->pName);
            continue;
        }

        data.pInterface->Shutdown();
        data.pDescriptor->DestroyPlugin(data.pInterface);
    }
}

void PluginCollection::UnloadPlugins()
{
    for (const PluginData& cData : m_pluginData)
    {
        FreeLibrary(static_cast<HMODULE>(cData.pModuleHandle));
    }
}

void PluginCollection::DumpLoadedPuginsToLog()
{
    for (const PluginData& cData : m_pluginData)
    {
    }
}
