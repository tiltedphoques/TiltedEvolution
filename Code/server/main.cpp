#include <stdafx.h>


#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog-inl.h>

#include <cxxopts.hpp>
#include <filesystem>
#include <GameServer.h>

int main(int argc, char** argv)
{
    std::error_code ec;
    std::filesystem::create_directory("logs", ec);

    auto rotatingLogger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/tp_game_server.log", 1048576 * 5, 3);
    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console->set_pattern("%^[%H:%M:%S] [%l]%$ %v");

    auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{ console, rotatingLogger });
    set_default_logger(logger);

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

    options.add_options()
        ("p,port", "port to run on", cxxopts::value<uint16_t>(port)->default_value("10578"), "N")
        ("root_password", "Admin password", cxxopts::value<>(adminPassword)->default_value(""), "N")
        ("premium", "Use the premium tick rates", cxxopts::value<bool>(premium)->default_value("false"), "true/false")
        ("h,help", "Display the help message")
        ("n,name", "Name to advertise to the public server list", cxxopts::value<>(name))
        ("l,log", "Log level.", cxxopts::value<>(logLevel)->default_value("info"), "trace/debug/info/warning/error/critical/off")
        ("t,token", "The token required to connect to the server, acts as a password", cxxopts::value<>(token));

    try
    {
        const auto result = options.parse(argc, argv);

        logger->set_level(spdlog::level::from_str(logLevel));

        if (result.count("help"))
        {
            std::cout << options.help({ "" }) << std::endl;
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

        GameServer server(port, premium, name.c_str(), token.c_str(), adminPassword.c_str());
        // things that need initialization post construction
        server.Initialize();

        while(server.IsListening())
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
