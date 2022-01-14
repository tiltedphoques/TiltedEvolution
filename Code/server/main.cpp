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
#include <base/CommandRegistry.h>

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
        console->set_pattern("%^[%H:%M:%S %l]%$ %v");

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

//https://www.oreilly.com/library/view/c-cookbook/0596007612/ch04s08.html
class StringTokenizer
{
  public:
    StringTokenizer(const std::string& s, const char* delim = NULL) : str_(s), count_(-1), begin_(0), end_(0)
    {

        if (!delim)
            delim_ = " \f\n\r\t\v"; // default to whitespace
        else
            delim_ = delim;

        // Point to the first token
        begin_ = str_.find_first_not_of(delim_);
        end_ = str_.find_first_of(delim_, begin_);
    }

    size_t countTokens()
    {
        if (count_ >= 0) // return if we've already counted
            return (count_);

        std::string::size_type n = 0;
        std::string::size_type i = 0;

        for (;;)
        {
            // advance to the first token
            if ((i = str_.find_first_not_of(delim_, i)) == std::string::npos)
                break;
            // advance to the next delimiter
            i = str_.find_first_of(delim_, i + 1);
            n++;
            if (i == std::string::npos)
                break;
        }
        return (count_ = n);
    }

    bool hasMoreTokens()
    {
        return (begin_ != end_);
    }

    void nextToken(std::string& s)
    {
        if (begin_ != std::string::npos && end_ != std::string::npos)
        {
            s = str_.substr(begin_, end_ - begin_);
            begin_ = str_.find_first_not_of(delim_, end_);
            end_ = str_.find_first_of(delim_, begin_);
        }
        else if (begin_ != std::string::npos && end_ == std::string::npos)
        {
            s = str_.substr(begin_, str_.length() - begin_);
            begin_ = str_.find_first_not_of(delim_, end_);
        }
    }

  private:
    StringTokenizer(){};
    std::string delim_;
    std::string str_;
    int count_;
    int begin_;
    int end_;
};

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
    server.Initialize();

    // Start the net thread.
    std::thread t([&]() {
        while (server.IsListening())
            server.Update();
    });
    t.detach();

    // Start the command handler.
    base::CommandRegistry r;
    fmt::print(">Welcome to the ST Server console\n");
    while (true)
    {
        fmt::print(">");
        std::string s;
        // TODO: ask for an alternative..
        std::getline(std::cin, s);

        if (s.length() <= 2 || s[0] != '/')
        {
            fmt::print(R"(Commands must begin with /)" "\n");
            continue;
        }

        std::vector<std::string> tokens;
        StringTokenizer tokenizer(&s[1]);
        while (tokenizer.hasMoreTokens())
        {
            tokenizer.nextToken(tokens.emplace_back());
        }

        std::string command = tokens[0];
        tokens.erase(tokens.begin());

        auto ss = r.TryExecuteCommand(command.c_str(), tokens);
        /* if (ss != base::CommandRegistry::Status::kSuccess)
        {
            fmt::print("No command {} found\n", &s[1]);
        }*/
    }

    return 0;
}
