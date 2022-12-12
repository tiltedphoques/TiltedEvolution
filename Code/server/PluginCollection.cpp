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

PluginCollection::PluginCollection() { CollectPlugins(std::filesystem::current_path()); }

PluginCollection::~PluginCollection() { UnloadPlugins(); }

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

    uint32_t loadCount = 0;
    for (const auto& path : canidates)
    {
        if (TryLoadPlugin(path))
            loadCount++;
    }

    spdlog::info("Loaded {} plugins", loadCount);

#if 0
    // TODO: warn if multiple extensions support a file type.
    for (const PluginData& it : m_pluginData)
    {
        if (it.)
    }
#endif
}

bool PluginCollection::TryLoadPlugin(const std::filesystem::path& aPath)
{
    auto doLoadPlugin = [this](void* apHandle, const uint8_t* apExport)
    {
        if (*reinterpret_cast<const uint32_t*>(apExport) != PluginAPI::kPluginMagic)
        {
            spdlog::error("TT_PLUGIN export is invalid (unknown magic)");
            return false;
        }

        auto* pDescriptor = reinterpret_cast<const PluginAPI::PluginDescriptor*>(apExport);
        if (pDescriptor->structSize != sizeof(PluginAPI::PluginDescriptor))
        {
            spdlog::error("TT_PLUGIN export is invalid (unknown size)");
            return false;
        }

        m_pluginData.emplace_back(
            apHandle, pDescriptor,
            /*Interface must be instantiated first*/ nullptr);
        return true;
    };

#if defined(_WIN32)
    auto pModuleHandle = LoadLibraryA(aPath.string().c_str());
    if (!pModuleHandle)
    {
        DWORD errorCode = GetLastError();

        LPSTR errorBuffer = NULL;
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errorBuffer, 0, NULL);

        // Print the error message
        spdlog::error("Error loading library {}: {}\n", aPath.string(), errorBuffer);
        LocalFree(errorBuffer);
        return false;
    }

    if (const uint8_t* pPluginDescriptor = reinterpret_cast<const uint8_t*>(GetProcAddress(pModuleHandle, "TT_PLUGIN")))
    {
        return doLoadPlugin(pModuleHandle, pPluginDescriptor);
    }
#elif defined(__linux__)
    auto pHandle = dlopen(path.c_str(), RTLD_LAZY);
    if (!pHandle)
    {
        spdlog::error("Failed to load plugin {}: {}", path.string(), dlerror());
        return false;
    }

    if (const uint8_t* pPluginDescriptor = reinterpret_cast<const uint8_t*>(dlsym(pHandle, "TT_PLUGIN")))
    {
        return doLoadPlugin(pHandle, pPluginDescriptor);
    }
#endif

    return false;
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

        auto* pInstance = data.pDescriptor->pCreatePlugin();
        if (!pInstance)
        {
            spdlog::error("Descriptor->CreatePlugin() for {} returned null. Did you forget to allocate the plugin instance?", data.pDescriptor->name.data());
            continue;
        }

        if (pInstance->GetVersion() > PluginAPI::kCurrentPluginInterfaceVersion)
        {
            spdlog::error("Plugin {} is using an unsupported interface version. Expected {}, got {}", data.pDescriptor->name.data(), PluginAPI::kCurrentPluginInterfaceVersion, pInstance->GetVersion());

            data.pDescriptor->pDestroyPlugin(pInstance);
            continue;
        }

        data.pInterface = pInstance;

        bool result = pInstance->Initialize();
        if (!result)
        {
            spdlog::error("plugin->Initialize() for {} returned false. Plugin initialization failed.", data.pDescriptor->name.data());

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
            spdlog::error("Unable to fetch interface for plugin {}.", data.pDescriptor->name.data());
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
