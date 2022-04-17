#pragma once

// spdlog
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog-inl.h>

#include <thread>
#include <TiltedCore/Filesystem.hpp>

#include <console/ConsoleRegistry.h>
#include <console/IniSettingsProvider.h>
#include <console/StringTokenizer.h>
#include <common/GameServerInstance.h>

#define GS_IMPORT extern __declspec(dllimport)

namespace fs = std::filesystem;

static constexpr char kConfigPathName[] = "config";

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

    TiltedPhoques::UniquePtr<IGameServerInstance> m_pGameServer;
    TiltedPhoques::UniquePtr<std::jthread> m_pConIOThread;
};

DediRunner* GetDediRunner();
