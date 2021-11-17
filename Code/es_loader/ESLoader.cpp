#include "ESLoader.h"

ESLoader::ESLoader(String aDirectory) 
    : m_directory(std::move(aDirectory))
{
}

bool ESLoader::BuildFileList()
{
    DEBUG_BREAK;

    if (!FindAndInsertFiles())
    {
        // error logged internally
        return false;
    }

    return true;
}

bool ESLoader::FindAndInsertFiles()
{
    DEBUG_BREAK;

    if (m_directory.empty())
    {
        spdlog::error("Directory string is empty.");
        return false;
    }


}

void ESLoader::SortAndLoadPluginsFromPluginsFile()
{
    DEBUG_BREAK;
}

