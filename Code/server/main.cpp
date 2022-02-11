#include <stdafx.h>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog-inl.h>

#include <GameServer.h>
#include <cxxopts.hpp>
#include <filesystem>

#include <Setting.h>
#include <fstream>
#include <simpleini/SimpleIni.h>

#include <TiltedCore/Filesystem.hpp>

#include <base/Check.h>
#include <base/simpleini/SimpleIni.h>
#include <console/ConsoleRegistry.h>
#include <console/IniSettingsProvider.h>
#include <console/StringTokenizer.h>

constexpr char kSettingsFileName[] =
#if SKYRIM
    "STServer.ini"
#elif FALLOUT4
    "FTServer.ini"
#else
    "Server.ini"
#endif
    ;

constexpr char kLogFileName[] =
#if SKYRIM
    "STServerOut.log"
#elif FALLOUT4
    "FTServerOut.log"
#else
    "TiltedGameServer.log"
#endif
    ;

// Its fine for us if several potential server instances read this, since its a tilted platform thing
// and therefore not considered game specific.
constexpr char kConfigPathName[] = "config";
constexpr char kEULAName[] = "EULA.txt";
constexpr char kEULAText[] = ";Please indicate your agreement to the Tilted platform service agreement\n"
                             ";by setting bConfirmEULA to true\n"
                             "[EULA]\n"
                             "bConfirmEULA=false";
constexpr char kConsoleOutName[] = "ConOut";

namespace fs = std::filesystem;

static Console::StringSetting sLogLevel{"sLogLevel", "Log level to print", "info"};
static Console::Setting bConsole{"bConsole", "Enable the console", true};

class DediRunner;

// LogInstance must be created for the Console Instance.
struct LogInstance
{
    static constexpr size_t kLogFileSizeCap = 1048576 * 5;

    LogInstance()
    {
        using namespace spdlog;

        std::error_code ec;
        fs::create_directory("logs", ec);

        auto fileOut =
            std::make_shared<sinks::rotating_file_sink_mt>(std::string("logs/") + kLogFileName, kLogFileSizeCap, 3);
        auto serverOut = std::make_shared<sinks::stdout_color_sink_mt>();
        serverOut->set_pattern("%^[%H:%M:%S] [%l]%$ %v");

        auto consoleOut = spdlog::stdout_color_mt(kConsoleOutName);
        consoleOut->set_pattern(">%$ %v");

        auto globalOut = std::make_shared<logger>("", sinks_init_list{serverOut, fileOut});
        globalOut->set_level(level::from_str(sLogLevel.value()));
        set_default_logger(globalOut);
    }

    ~LogInstance()
    {
        spdlog::shutdown();
    }
};

struct SettingsInstance
{
    SettingsInstance()
    {
        m_path = fs::current_path() / kConfigPathName / kSettingsFileName;
        if (!fs::exists(m_path))
        {
            create_directory(fs::current_path() / kConfigPathName);
            Console::SaveSettingsToIni(m_path);
            return;
        }
        Console::LoadSettingsFromIni(m_path);
    }

    ~SettingsInstance()
    {
        Console::SaveSettingsToIni(m_path);
    }

  private:
    fs::path m_path;
};

static DediRunner* s_pRunner{nullptr};

// Frontend class for the dedicated terminal based server
class DediRunner
{
  public:
    DediRunner(int argc, char** argv);
    ~DediRunner() = default;

    void RunGSThread();
    void StartTerminalIO();
    void RequestKill();

  private:
    static void PrintExecutorArrowHack();

  private:
    fs::path m_configPath;
    // Order here matters for constructor calling order.
    SettingsInstance m_settings;
    LogInstance m_logInstance;
    GameServer m_gameServer;
    Console::ConsoleRegistry m_console;
    std::unique_ptr<std::jthread> m_pConIOThread;
};

DediRunner::DediRunner(int argc, char** argv) : m_gameServer(&m_console), m_console(kConsoleOutName)
{
    s_pRunner = this;

    // Post construction init stuff.
    m_gameServer.Initialize();
}

void DediRunner::PrintExecutorArrowHack()
{
    // Force:
    // This is a hack to get the executor arrow.
    // If you find a way to do this through the ConOut log channel
    // please let me know (The issue is the forced formatting for that channel)
    // and the forced null termination.
    fmt::print(">>>");
}

void DediRunner::RunGSThread()
{
    while (m_gameServer.IsListening())
    {
        m_gameServer.Update();
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

        while (m_gameServer.IsRunning())
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
    m_gameServer.Kill();

    if (m_pConIOThread)
    {
        auto wait = std::move(m_pConIOThread);
        TP_UNUSED(wait);
    }
    // work around 
    // https://cdn.discordapp.com/attachments/675107843573022779/941772837339930674/unknown.png
    // being set.
#if defined(_WIN32)
    if (IsDebuggerPresent())
    {
        std::this_thread::sleep_for(300ms);
    }
#endif
}

static bool RegisterQuitHandler()
{
#if defined(_WIN32)
    return SetConsoleCtrlHandler(
        [](DWORD aType) 
        {
            switch (aType)
            {
            case CTRL_C_EVENT:
            case CTRL_CLOSE_EVENT:
            case CTRL_BREAK_EVENT:
            case CTRL_LOGOFF_EVENT:
            case CTRL_SHUTDOWN_EVENT: 
            {
                if (s_pRunner)
                {
                    s_pRunner->RequestKill();
                    return TRUE;
                }

                // if the user kills during the ctor we deny the request
                // to save our dear life.
                return FALSE;
            }
            default:
                return FALSE;
            }
        },
        TRUE);
#endif
    return true;
}

static void ShittyFileWrite(const std::filesystem::path& aPath, const char* const acData)
{
    // TODO: Get rid of this, its horrible.
    std::ofstream myfile(aPath.c_str());
    myfile << acData;
    myfile.close();
}

static bool IsEULAAccepted()
{
    const auto path = fs::current_path() / kConfigPathName / kEULAName;
    if (!exists(path))
    {
        create_directory(fs::current_path() / kConfigPathName);
        ShittyFileWrite(path, kEULAText);
        return false;
    }

    const auto data = LoadFile(path);
    CSimpleIni si;
    if (si.LoadData(data.c_str()) != SI_OK)
        return false;

    return si.GetBoolValue("EULA", "bConfirmEULA", false);
}

int main(int argc, char** argv)
{
    if (!IsEULAAccepted())
    {
        fmt::print("Please accept the EULA by setting bConfirmEULA to true in EULA.txt");
        return 0;
    }

    RegisterQuitHandler();

    // Keep stack free.
    const auto cpRunner{std::make_unique<DediRunner>(argc, argv)};
    if (bConsole)
    {
        cpRunner->StartTerminalIO();
    }
    cpRunner->RunGSThread();

    return 0;
}
