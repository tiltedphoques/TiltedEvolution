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
            m_masterFiles[line] = standardId;
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
    Map<uint32_t, NPC> npcs{};

    for (Plugin& plugin : m_loadOrder)
    {
        fs::path pluginPath = GetPath(plugin.m_filename);
        if (pluginPath.empty())
        {
            spdlog::error("Path to plugin file not found: {}", plugin.m_filename);
            continue;
        }

        TESFile pluginFile(m_masterFiles);
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
        //spdlog::info("Climate count in {}: {}", plugin.m_filename, pluginClimates.size());

        const Map<uint32_t, NPC>& pluginNpcs = pluginFile.GetNPCs();
        auto npc = pluginNpcs.find(0x13480);
        if (npc != std::end(pluginNpcs))
            spdlog::info("Found Faendal! {}", npc->second.m_baseStats.IsRespawn());
        for (auto& [formId, npc] : pluginNpcs)
        {
            npcs[formId] = npc;
        }
        npcs.insert(pluginNpcs.begin(), pluginNpcs.end());
    }

    spdlog::info("All NPCs:");
    for (auto& [formId, npc] : npcs)
    {
        if (formId == 0x13480)
            spdlog::info("\tIsRespawn? {} ({:X})", npc.m_baseStats.IsRespawn(), formId);
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

String ESLoader::ReadZString(Buffer::Reader& aReader) noexcept
{
    String zstring = String(reinterpret_cast<const char*>(aReader.GetDataAtPosition()));
    aReader.Advance(zstring.size() + 1);
    return zstring;
}

String ESLoader::ReadWString(Buffer::Reader& aReader) noexcept
{
    // TODO: docs don't mention wstring being an actual wide string, test this
    uint16_t stringLength = 0;
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&stringLength), 2);
    String wstring = String(reinterpret_cast<const char*>(aReader.GetDataAtPosition()), stringLength);
    aReader.Advance(stringLength);
    return wstring;
}

