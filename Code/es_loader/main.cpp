#include "ESLoader.h"

int main(int argc, char** argv)
{
    ESLoader loader("Data\\");
    if (!loader.BuildFileList())
    {
        spdlog::error("Failed to build file list.");
        return -1;
    }

    spdlog::info("Build file list succeeded.");

    spdlog::info("ESM filenames:");
    for (auto esmFilename : loader.m_esmFilenames)
    {
        spdlog::info("\t{}", esmFilename);
    }

    spdlog::info("ESP filenames:");
    for (auto espFilename : loader.m_espFilenames)
    {
        spdlog::info("\t{}", espFilename);
    }

    spdlog::info("ESL filenames:");
    for (auto eslFilename : loader.m_eslFilenames)
    {
        spdlog::info("\t{}", eslFilename);
    }

    return 0;
}

