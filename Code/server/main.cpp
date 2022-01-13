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
#include <base/IniSettingsProvider.h>

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

static base::StringSetting sLogLevel{"sLogLevel", "Log level to print", "info"};

struct LogScope
{
    LogScope()
    {
        std::error_code ec;
        fs::create_directory("logs", ec);

        auto rotatingLogger =
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/tp_game_server.log", 1048576 * 5, 3);
        auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console->set_pattern("%^[%H:%M:%S] [%l]%$ %v");

        auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{console, rotatingLogger});
        logger->set_level(spdlog::level::from_str(sLogLevel.value()));
        set_default_logger(logger);
    }

    ~LogScope()
    {
        spdlog::shutdown();
    }
};

struct SettingsScope
{
    SettingsScope()
    {
        m_Path = fs::current_path() / kSettingsFileName;
        if (!fs::exists(m_Path))
        {
            // Write defaults
            base::SaveSettingsToIni(m_Path);
            return;
        }

        base::LoadSettingsFromIni(m_Path);
    }

    ~SettingsScope()
    {
        base::SaveSettingsToIni(m_Path);
    }

  private:
    fs::path m_Path;
};

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

    SettingsScope sscope;
    (void)sscope;

    LogScope lscope;
    (void)lscope;

    GameServer server;
    // things that need initialization post construction
    server.Initialize();

    while (server.IsListening())
        server.Update();

    return 0;
}
