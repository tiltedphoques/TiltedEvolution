#include <stdafx.h>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog-inl.h>

#include <GameServer.h>
#include <cxxopts.hpp>
#include <filesystem>

#include <fstream>
#include <Setting.h>
#include <simpleini/SimpleIni.h>

#include <TiltedCore/Filesystem.hpp>

constexpr char kSettingsFileName[] =
#if SKYRIM
    "STServer.ini"
#elif FALLOUT4
    "FTServer.ini"
#else
    "Server.ini"
#endif
    ;

namespace fs = std::filesystem;

static std::shared_ptr<spdlog::logger> InitLogging()
{
    std::error_code ec;
    fs::create_directory("logs", ec);

    auto rotatingLogger =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/tp_game_server.log", 1048576 * 5, 3);
    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console->set_pattern("%^[%H:%M:%S] [%l]%$ %v");

    auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{console, rotatingLogger});
    set_default_logger(logger);
    return logger;
}

static void DoWriteFile(const char *path, const std::string& data)
{
    std::ofstream myfile;
    myfile.open(path);
    myfile << data.c_str();
    myfile.close();
}

static void ParseSettingsList()
{
    CSimpleIni ini;
    // Write defaults
    auto configPath = fs::current_path() / kSettingsFileName;
    if (!fs::exists(configPath))
    {
        SettingBase::VisitAll([&](SettingBase* setting) {
            switch (setting->type)
            {
            case SettingBase::Type::kBoolean:
                ini.SetBoolValue("test", setting->name, setting->data.as_boolean);
                break;
            case SettingBase::Type::kInt:


                // TODO: handle unsigned types.
                ini.SetLongValue("test", setting->name, setting->data.as_int32);
                break;
            case SettingBase::Type::kFloat:
                ini.SetDoubleValue("test", setting->name, setting->data.as_float);
                break;
            default:
                assert(true);
            }
        });

        std::string buf;
        ini.Save(buf, true);

        DoWriteFile(configPath.u8string().c_str(), buf);
        return;
    }

    {
        auto buf = TiltedPhoques::LoadFile(configPath);
        ini.LoadData(buf.c_str());
    }

     SettingBase::VisitAll([&](SettingBase* setting) {
        switch (setting->type)
        {
        case SettingBase::Type::kBoolean:
            setting->data.as_boolean = ini.GetBoolValue("test", setting->name, setting->data.as_boolean);
            break;
        case SettingBase::Type::kInt:
            setting->data.as_int32 = ini.GetLongValue("test", setting->name, setting->data.as_int32);
            break;
        case SettingBase::Type::kFloat:
            setting->data.as_float = ini.GetDoubleValue("test", setting->name, setting->data.as_float);
            break;
        default:
            assert(true);
        }
    });
}

int main(int argc, char** argv)
{
    auto logger = InitLogging();
    ParseSettingsList();

    cxxopts::Options options(argv[0], "Game server for "
#if SKYRIM
                                      "Skyrim"
#elif FALLOUT4
                                      "Fallout 4"
#else
                                      "Unknown game"
#endif
    );

    uint16_t port = 10578;
    bool premium = false;
    std::string name, token, logLevel, adminPassword;

    options.add_options()("p,port", "port to run on", cxxopts::value<uint16_t>(port)->default_value("10578"), "N")(
        "root_password", "Admin password", cxxopts::value<>(adminPassword)->default_value(""),
        "N")("premium", "Use the premium tick rates", cxxopts::value<bool>(premium)->default_value("false"),
             "true/false")("h,help", "Display the help message")(
        "n,name", "Name to advertise to the public server list",
        cxxopts::value<>(name))("l,log", "Log level.", cxxopts::value<>(logLevel)->default_value("info"),
                                "trace/debug/info/warning/error/critical/off")(
        "t,token", "The token required to connect to the server, acts as a password", cxxopts::value<>(token));

    try
    {
        const auto result = options.parse(argc, argv);

        logger->set_level(spdlog::level::from_str(logLevel));

        if (result.count("help"))
        {
            std::cout << options.help({""}) << std::endl;
            return 0;
        }

        if (!token.empty() && !name.empty())
        {
            throw std::runtime_error("A named server cannot have a token set!");
        }

        if (!token.empty() && token == adminPassword)
        {
            throw std::runtime_error("The root password cannot be the same as the token!");
        }

        GameServer server(name.c_str(), token.c_str(), adminPassword.c_str());
        // things that need initialization post construction
        server.Initialize();

        while (server.IsListening())
            server.Update();
    }
    catch (const cxxopts::OptionException& e)
    {
        std::cout << "Options parse error: " << e.what() << std::endl;
        return -1;
    }
    catch (const std::runtime_error& e)
    {
        spdlog::error(e.what());
    }

    spdlog::shutdown();

    return 0;
}
