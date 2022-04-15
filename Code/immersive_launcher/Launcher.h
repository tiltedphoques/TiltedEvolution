#pragma once

#include "Loader/ExeLoader.h"
#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Stl.hpp>

namespace launcher
{
namespace fs = std::filesystem;

enum class Result
{
    kSuccess,
    kBadPlatform,
    kBadInstall
};

// stays alive through the entire duration of the game.
struct LaunchContext
{
    fs::path exePath;
    fs::path gamePath;
    TiltedPhoques::String Version;
    ExeLoader::TEntryPoint gameMain = nullptr;
};

LaunchContext* GetLaunchContext();

bool LoadProgram(LaunchContext&);
int StartUp(int argc, char** argv);

void InitClient();
} // namespace launcher
