#pragma once

#include <spdlog/spdlog.h>

#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Platform.hpp>
#include "Loader/ExeLoader.h"

namespace fs = std::filesystem;

// stays alive through the entire duration of the game.
struct LaunchContext
{
    fs::path exePath;
    fs::path gamePath;
    ExeLoader::TEntryPoint gameMain = nullptr;
};

LaunchContext* GetLaunchContext();

void Bootstrap();
void RunClient();
