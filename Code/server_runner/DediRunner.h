#pragma once

// spdlog
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <thread>
#include <TiltedCore/Filesystem.hpp>

#include <BuildInfo.h>
#include <console/ConsoleRegistry.h>
#include <console/IniSettingsProvider.h>
#include <console/StringTokenizer.h>
#include <common/GameServerInstance.h>

#ifdef _WIN32
#define GS_IMPORT extern __declspec(dllimport)
#else
#define GS_IMPORT extern __attribute__((visibility("default")))
#endif

namespace fs = std::filesystem;

static constexpr char kConfigPathName[] = "config";
static constexpr char KCompilerStopThisBullshit[] = "ConOut";
static constexpr char kBuildTag[]{BUILD_BRANCH "@" BUILD_COMMIT};

// Frontend class for the dedicated terminal based server
class DediRunner
{
  public:
    DediRunner(int argc, char** argv);
    ~DediRunner();

    void RunGSThread();
    void StartTerminalIO();
    void RequestKill();

  private:
    static void PrintExecutorArrowHack();

    void LoadSettings();

  private:
    //fs::path m_configPath;
    // Order here matters for constructor calling order.
    fs::path m_SettingsPath;
    Console::ConsoleRegistry m_console;
    TiltedPhoques::UniquePtr<std::jthread> m_pConIOThread;
    TiltedPhoques::UniquePtr<IGameServerInstance> m_pServerInstance;
};

DediRunner* GetDediRunner() noexcept;
