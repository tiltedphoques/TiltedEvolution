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
    bool FindAndInsertFiles();
    void SortAndLoadPluginsFromPluginsFile();

private:
    String m_directory;
    Vector<TESFile> m_plugins;
};
