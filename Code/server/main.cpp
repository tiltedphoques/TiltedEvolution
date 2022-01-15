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
#include <console/IniSettingsProvider.h>
#include <console/ConsoleRegistry.h>
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

// Its fine for us if several potential server instances read this, since its a tilted platform thing
// and therefore not considered game specific.
constexpr char kEULAName[] = "EULA.txt";
constexpr char kEULAText[] = ";Please indicate your agreement to the Tilted platform service agreement\n"
                             ";by setting bConfirmEULA to true\n"
                             "[EULA]\n"
                             "bConfirmEULA=false";

namespace fs = std::filesystem;

static console::StringSetting sLogLevel{"sLogLevel", "Log level to print", "info"};

struct LogInstance
{
    static constexpr size_t kLogFileSizeCap = 1048576 * 5;

    LogInstance()
    {
        using namespace spdlog;

        std::error_code ec;
        fs::create_directory("logs", ec);

        auto rotatingLogger =
            std::make_shared<sinks::rotating_file_sink_mt>("logs/tp_game_server.log", kLogFileSizeCap, 3);

        auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console->set_pattern("%^[%H:%M:%S %l]%$ %v");

        auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{console, rotatingLogger});
        logger->set_level(spdlog::level::from_str(sLogLevel.value()));
        set_default_logger(logger);
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
            console::SaveSettingsToIni(m_Path);
            return;
        }
        console::LoadSettingsFromIni(m_Path);
    }

    ~SettingsInstance()
    {
        console::SaveSettingsToIni(m_Path);
    }

  private:
    fs::path m_Path;
};

// Frontend class for the dedicated terminal based server
class DediRunner
{
  public:
    DediRunner(int argc, char ** argv);
    ~DediRunner() = default;

    void StartGSThread();
    void RunTerminalIO();

  private:
    fs::path m_configPath;
    // Order here matters for constructor calling order.
    SettingsInstance m_settings;
    LogInstance m_logInstance;
    GameServer m_gameServer;
    console::ConsoleRegistry m_consoleReg;
};

DediRunner::DediRunner(int argc, char ** argv)
{
    // Post construction init stuff.
    m_gameServer.Initialize();
}

void DediRunner::StartGSThread()
{
    std::thread t([&]() {
        while (m_gameServer.IsListening())
        {
        // TODO: drain command queue
            m_gameServer.Update();
        }
    });
    t.detach();
}

void DediRunner::RunTerminalIO()
{ 
    fmt::print(">Welcome to the ST Server console\n");
    while (true)
    {
        fmt::print(">");
        // TODO: consider wide IO for terminal
        std::string s;
        std::getline(std::cin, s);

        if (s.length() <= 2 || s[0] != '/')
        {
            fmt::print("Commands must begin with /");
            continue;
        }

        std::vector<std::string> tokens;
        console::StringTokenizer tokenizer(&s[1]);
        while (tokenizer.HasMore())
        {
            tokenizer.GetNext(tokens.emplace_back());
        }

        std::string command = tokens[0];
        tokens.erase(tokens.begin());

        auto ss = m_consoleReg.ScheduleCommand(command.c_str(), tokens);
        /* if (ss != base::CommandRegistry::Status::kSuccess)
        {
            fmt::print("No command {} found\n", &s[1]);
        }*/
        // Unknown command. Type /help for help.
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

    DediRunner runner(argc, argv);
    runner.StartGSThread();
    runner.RunTerminalIO();
    return 0;
}
