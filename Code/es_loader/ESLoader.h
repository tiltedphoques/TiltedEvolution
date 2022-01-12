#pragma once

#include <stdafx.h>
#include <TESFile.h>

class ESLoader
{
public:
    ESLoader() = delete;
    ESLoader(String aDirectory);

//private:
    void FindFiles();
    void LoadFiles();

//private:
    String m_directory;
    Vector<std::filesystem::path> m_esmFilenames;
    Vector<std::filesystem::path> m_espFilenames;
    Vector<std::filesystem::path> m_eslFilenames;
    Vector<TESFile> m_standardPlugins;
    Vector<TESFile> m_lightPlugins;
};
