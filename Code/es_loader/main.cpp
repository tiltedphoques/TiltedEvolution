#include "ESLoader.h"

int main(int argc, char** argv)
{
    ESLoader loader("game_files");
    if (!loader.BuildFileList())
    {
        spdlog::error("Failed to build file list.");
        return -1;
    }

    spdlog::info("Build file list succeeded.");

    return 0;
}

