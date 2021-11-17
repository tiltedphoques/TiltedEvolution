#pragma once

#include <stdafx.h>
#include <TESFile.h>

class ESLoader
{
public:
    ESLoader() = delete;
    ESLoader(String aDirectory);

    bool BuildFileList();

private:
    bool FindFiles();
    bool SortPlugins();

private:
    String m_directory;
    Vector<String> m_esmFilenames;
    Vector<String> m_espFilenames;
    Vector<String> m_eslFilenames;
    Vector<TESFile> m_standardPlugins;
    Vector<TESFile> m_lightPlugins;
};
