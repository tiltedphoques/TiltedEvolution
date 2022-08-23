#pragma once

// spdlog
#include <spdlog/spdlog.h>

#include <thread>

#include <BuildInfo.h>
#include <uv.h>
#include <console/ConsoleRegistry.h>
#include <console/IniSettingsProvider.h>
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
struct DediRunner
{
    DediRunner(int argc, char** argv);
    ~DediRunner();

    void RunGSThread();
    void StartTerminalIO();
    void RequestKill();
    void HandleConsole(const TiltedPhoques::String& acCommand);

private:
    static void PrintExecutorArrowHack();

    void LoadSettings(int argc, char** argv);

    static void ReadStdin(uv_stream_t* apStream, ssize_t aRead, const uv_buf_t* acpBuffer);
    static void AllocateBuffer(uv_handle_t* apHandle, size_t aSuggestedSize, uv_buf_t* apBuffer);

private:
    //fs::path m_configPath;
    // Order here matters for constructor calling order.
    uv_loop_t m_loop;
    uv_tty_t m_tty;
    fs::path m_SettingsPath;
    bool m_useIni{ false };
    Console::ConsoleRegistry m_console;
    TiltedPhoques::UniquePtr<IGameServerInstance> m_pServerInstance;
};

DediRunner* GetDediRunner() noexcept;
