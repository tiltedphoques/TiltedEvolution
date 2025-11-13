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
    bool isLoaded{false};
    ExeLoader::TEntryPoint gameMain = nullptr;

    void SetLoaded() { isLoaded = true; }   // If loaded, need to spoof GetModuleFileName*(nullptr)
    bool GetLoaded();
};

LaunchContext* GetLaunchContext();

bool LoadProgram(LaunchContext&);
int StartUp(int argc, char** argv);

void InitClient();

bool HandleArguments(int, char**, bool&);

} // namespace launcher
