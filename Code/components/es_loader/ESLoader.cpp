#include "ESLoader.h"

#include <filesystem>
#include <fstream>

#include <Records/REFR.h>
#include <Records/CLMT.h>
#include <Records/NPC.h>

ESLoader::ESLoader(String aDirectory) 
    : m_directory(std::move(aDirectory))
{
    FindFiles();
    LoadFiles();
}

void ESLoader::FindFiles()
{
    if (m_directory.empty())
    {
        spdlog::error("Directory string is empty.");
        return;
    }

    bool result = LoadLoadOrder();
}

bool ESLoader::LoadLoadOrder()
{
    std::ifstream loadOrderFile;
    String loadOrderPath = m_directory + "loadorder.txt";
    loadOrderFile.open(loadOrderPath);
    if (loadOrderFile.fail())
    {
        spdlog::error("Failed to open loadorder.txt");
        return false;
    }

    uint8_t standardId = 0x0;
    uint16_t liteId = 0x0;

    while (!loadOrderFile.eof())
    {
        String line;
        std::getline(loadOrderFile, line);
        if (line[0] == '#' || line.empty())
            continue;

        Plugin plugin;
        plugin.m_filename = line;

        char extensionType = line.back();
        switch (extensionType)
        {
        case 'm':
        case 'p':
            plugin.m_standardId = standardId;
            standardId += 0x01;
            plugin.m_isLite = false;
            m_loadOrder.push_back(plugin);
            break;
        case 'l':
            plugin.m_liteId = liteId;
            liteId += 0x0001;
            plugin.m_isLite = true;
            m_loadOrder.push_back(plugin);
            break;
        default:
            spdlog::error("Extension in loadorder.txt not recognized: {}", line);
        }
    }

    return true;
}

void ESLoader::LoadFiles()
{
    Map<uint32_t, CLMT> climates{};

    for (Plugin& plugin : m_loadOrder)
    {
        fs::path pluginPath = GetPath(plugin.m_filename);
        if (pluginPath.empty())
        {
            spdlog::error("Path to plugin file not found: {}", plugin.m_filename);
            continue;
        }

        TESFile pluginFile{};
        if (plugin.IsLite())
            pluginFile.Setup(plugin.m_liteId);
        else
            pluginFile.Setup(plugin.m_standardId);

        bool loadResult = pluginFile.LoadFile(pluginPath);

        if (!loadResult)
            continue;

        pluginFile.IndexRecords();

        const Map<uint32_t, CLMT>& pluginClimates = pluginFile.GetClimates();
        climates.insert(pluginClimates.begin(), pluginClimates.end());
    }

    spdlog::info("All climates:");
    for (auto& [formId, climate] : climates)
    {
        spdlog::info("\t{} ({:X})", climate.m_editorId);
    }
}

// TODO: std::optional
fs::path ESLoader::GetPath(String& aFilename)
{
    for (const auto& entry : fs::directory_iterator(m_directory))
    {
        String filename = entry.path().filename().string().c_str();
        if (filename == aFilename)
            return entry.path();
    }

    return fs::path();
}

String ESLoader::LoadZString(Buffer::Reader& aReader) noexcept
{
    String zstring = String(reinterpret_cast<const char*>(aReader.GetDataAtPosition()));
    aReader.Advance(zstring.size() + 1);
    return zstring;
}

