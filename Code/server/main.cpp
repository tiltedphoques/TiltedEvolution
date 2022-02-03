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
constexpr char kEULAName[] = "EULA.txt";
constexpr char kEULAText[] = ";Please indicate your agreement to the Tilted platform service agreement\n"
                             ";by setting bConfirmEULA to true\n"
                             "[EULA]\n"
                             "bConfirmEULA=false";
constexpr char kConsoleOutName[] = "ConOut";

namespace fs = std::filesystem;

static Console::StringSetting sLogLevel{"sLogLevel", "Log level to print", "info"};

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
        m_Path = fs::current_path() / kSettingsFileName;
        if (!fs::exists(m_Path))
        {
            Console::SaveSettingsToIni(m_Path);
            return;
        }
        Console::LoadSettingsFromIni(m_Path);
    }

    ~SettingsInstance()
    {
        Console::SaveSettingsToIni(m_Path);
    }

  private:
    fs::path m_Path;
};

// Frontend class for the dedicated terminal based server
class DediRunner
{
  public:
    DediRunner(int argc, char** argv);
    ~DediRunner() = default;

    void StartGSThread();
    void RunTerminalIO();

  private:
    fs::path m_configPath;
    // Order here matters for constructor calling order.
    SettingsInstance m_settings;
    LogInstance m_logInstance;
    GameServer m_gameServer;
    Console::ConsoleRegistry m_console;
};

DediRunner::DediRunner(int argc, char** argv) : m_console(kConsoleOutName)
{
    // Post construction init stuff.
    m_gameServer.Initialize();
}

void DediRunner::StartGSThread()
{
    std::thread t([&]() {
        while (m_gameServer.IsListening())
        {
            m_gameServer.Update();
            if (m_console.Update())
            {
                // Force:
                // This is a hack to get the executor arrow.
                // If you find a way to do this through the ConOut log channel
                // please let me know (The issue is the forced formatting for that channel)
                // and the forced null termination.
                fmt::print(">");
            }
        }
    });
    t.detach();
}

void DediRunner::RunTerminalIO()
{
    spdlog::get("ConOut")->info("Welcome to the ST Server console");
    fmt::print(">");
    while (true)
    {
        std::string s;
        std::getline(std::cin, s);
        m_console.TryExecuteCommand(s);
    }
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
    auto path = fs::current_path() / kEULAName;
    if (!fs::exists(path))
    {
        ShittyFileWrite(path, kEULAText);
        return false;
    }

    auto data = TiltedPhoques::LoadFile(path);
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

    // Keep stack free.
    auto runner{std::make_unique<DediRunner>(argc, argv)};
    runner->StartGSThread();
    runner->RunTerminalIO();
    return 0;
}
