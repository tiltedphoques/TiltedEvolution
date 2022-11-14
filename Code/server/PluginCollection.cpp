#include "PluginCollection.h"
#include "Pch.h"

#if defined(__linux__)
#include <dlfcn.h>
#endif

static constexpr char kPluginFolderName[] = "plugins";

static constexpr char kNativePluginExtension[] =
#if defined(_WIN32)
    ".dll";
#elif defined(__linux__)
    ".so";
#endif

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
#if defined(_WIN32)
        if (auto pHandle = LoadLibraryW(path.c_str()))
#elif defined(__linux__)
        if (auto pHandle = dlopen(path.c_str(), RTLD_LAZY))
#endif
        {
            if (uint8_t* pPluginDescriptor =
#if defined(_WIN32)
                    reinterpret_cast<uint8_t*>(GetProcAddress(pHandle, "TT_PLUGIN")))
#elif defined(__linux__)
                    reinterpret_cast<uint8_t*>(dlsym(pHandle, "TT_PLUGIN")))
#endif
            {
                if (*reinterpret_cast<uint32_t*>(pPluginDescriptor) != kPluginMagic)
                {
                    spdlog::error("TT_PLUGIN export is invalid");
                    continue;
                }

                m_pluginData.emplace_back(pHandle, reinterpret_cast<PluginDescriptor*>(pPluginDescriptor),
                                          /*Interface must be instantiated first*/ nullptr);
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
        PluginInterface001* pInstance = data.pDescriptor->pCreatePlugin();
        if (!pInstance)
        {
            spdlog::error(
                "Descriptor->CreatePlugin() for {} returned null. Did you forget to allocate the plugin instance?",
                data.pDescriptor->pluginName.data());
            continue;
        }

        data.pInterface = pInstance;

        bool result = pInstance->Initialize();
        if (!result)
        {
            spdlog::error("plugin->Initialize() for {} returned false. Plugin initialization failed.",
                          data.pDescriptor->pluginName.data());

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
            spdlog::error("Unable to fetch interface for plugin {}.", data.pDescriptor->pluginName.data());
            continue;
        }

        data.pInterface->Shutdown();
        data.pDescriptor->pDestroyPlugin(data.pInterface);
    }
}

void PluginCollection::UnloadPlugins()
{
    for (const PluginData& cData : m_pluginData)
    {
#if defined(_WIN32)
        FreeLibrary(static_cast<HMODULE>(cData.pModuleHandle));
#elif defined(__linux__)
        dlclose(cData.pModuleHandle);
#endif
    }
}

void PluginCollection::DumpLoadedPuginsToLog()
{
    for (const PluginData& cData : m_pluginData)
    {
    }
}
