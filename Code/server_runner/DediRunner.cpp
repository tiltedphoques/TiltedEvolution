
#include "DediRunner.h"

#include <chrono>
#include <iostream>
#include <spdlog/spdlog.h>

namespace
{
constexpr char kSettingsFileName[] =
#if TARGET_ST
    "STServer.ini"
#elif TARGET_FT
    "FTServer.ini"
#else
    "Server.ini"
#endif
    ;

DediRunner* s_pRunner{nullptr};
} // namespace

using namespace std::chrono_literals;

class GameServer;

GS_IMPORT IGameServerInstance* CreateGameServer(Console::ConsoleRegistry& conReg);
GS_IMPORT void DestroyGameServer(IGameServerInstance* apServer);

constexpr char kConsoleOutName[] = "ConOut";

Console::Setting bConsole{"bConsole", "Enable the console", true};


DediRunner* GetDediRunner()
{
    return s_pRunner;
}

DediRunner::DediRunner(int argc, char** argv) : m_console(kConsoleOutName)
{
    LoadSettings();
    s_pRunner = this;

    m_pServerInstance = CreateGameServer(m_console);
}

DediRunner::~DediRunner()
{
    DestroyGameServer(m_pServerInstance);
    Console::SaveSettingsToIni(m_console, m_SettingsPath);
}

void DediRunner::LoadSettings()
{
    m_SettingsPath = fs::current_path() / kConfigPathName / kSettingsFileName;
    if (!fs::exists(m_SettingsPath))
    {
        // there is a bug in here... waiting to be found
        // since we dont register our settings till later, so the server settings might be... missing??
        create_directory(fs::current_path() / kConfigPathName);
        Console::SaveSettingsToIni(m_console, m_SettingsPath);
        return;
    }
    Console::LoadSettingsFromIni(m_console, m_SettingsPath);
}

void DediRunner::PrintExecutorArrowHack()
{
    // Force:
    // This is a hack to get the executor arrow.
    // If you find a way to do this through the ConOut log channel
    // please let me know (The issue is the forced formatting for that channel and the forced null termination)
    fmt::print(">>>");
}

void DediRunner::RunGSThread()
{
    while (m_pServerInstance->IsListening())
    {
        m_pServerInstance->Update();
        if (bConsole && m_console.Update())
        {
            PrintExecutorArrowHack();
        }
    }
}

void DediRunner::StartTerminalIO()
{
    m_pConIOThread.reset(new std::jthread([&]() {
        spdlog::get("ConOut")->info("Server console");
        PrintExecutorArrowHack();

        while (m_pServerInstance->IsRunning())
        {
            std::string s;
            std::getline(std::cin, s);
            if (!m_console.TryExecuteCommand(s))
            {
                // we take the opportunity here and insert it directly
                // else we will be lacking it, and we want to avoid testing the queue size after
                // insert due to race condition.
                PrintExecutorArrowHack();
            }

            // best way to ensure this thread exits immediately tbh ¯\_("")_/¯.
            if (s == "/quit")
                return;
        }
    }));
}

void DediRunner::RequestKill()
{
    m_pServerInstance->Shutdown();

    auto wait = std::move(m_pConIOThread);
    (void)(wait);

#if defined(_WIN32)
    // work around Control Handler exception (Control-C) being set
    // https://cdn.discordapp.com/attachments/675107843573022779/941772837339930674/unknown.png
    // being set.
    if (IsDebuggerPresent())
    {
        std::this_thread::sleep_for(300ms);
    }
#endif
}
