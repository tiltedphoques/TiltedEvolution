#pragma once

#include <stdafx.h>
#include <TESFile.h>

class ESLoader
{
public:

    ESLoader() = delete;
    ESLoader(String aDirectory);

    // TODO: yes, this map thing is awful, but i still need to sort out how to do the gameid thingy
    template<class T> 
    Map<String, Vector<T>> GetRecords() noexcept;

private:
    void FindFiles();
    void LoadFiles();

    String m_directory;
    Vector<std::filesystem::path> m_esmFilenames;
    Vector<std::filesystem::path> m_espFilenames;
    Vector<std::filesystem::path> m_eslFilenames;
    Vector<TESFile> m_standardPlugins;
    Vector<TESFile> m_lightPlugins;

    Map<Record*, SharedPtr<Buffer>> m_decompressedChunksCache{};
};
