
#include <TiltedCore/Filesystem.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <Setting.h>
#include <base/Check.h>
#include <base/simpleini/SimpleIni.h>
#include <common/GameServerInstance.h>

#include "DediRunner.h"

#include <BuildInfo.h>

constexpr char kBuildTag[]{BUILD_BRANCH "@" BUILD_COMMIT};

GS_IMPORT const char* GetBuildTag();
GS_IMPORT void SetDefaultLogger(std::shared_ptr<spdlog::logger> default_logger);

namespace
{
constexpr char kLogFileName[] =
#if TARGET_ST
    "STServerOut.log"
#elif TARGET_FT
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

Console::StringSetting sLogLevel{"sLogLevel", "Log level to print", "info"};
using namespace std::chrono_literals;
} // namespace

extern Console::Setting<bool> bConsole;

// LogInstance must be created for the Console Instance.
struct LogInstance
{
    static constexpr size_t kLogFileSizeCap = 1048576 * 5;

    LogInstance()
    {
        using namespace spdlog;

        std::error_code ec;
        fs::create_directory("logs", ec);

        auto consoleOut = spdlog::stdout_color_mt(kConsoleOutName);
        consoleOut->set_pattern(">%$ %v");

        auto fileOut =
            std::make_shared<sinks::rotating_file_sink_mt>(std::string("logs/") + kLogFileName, kLogFileSizeCap, 3);
        auto serverOut = std::make_shared<sinks::stdout_color_sink_mt>();
        serverOut->set_pattern("%^[%H:%M:%S] [%l]%$ %v");
        auto globalOut = std::make_shared<logger>("", sinks_init_list{serverOut, fileOut});
        globalOut->set_level(level::from_str(sLogLevel.value()));

        // spdlog is not designed for multi library use, so we need to make it behave.
        SetDefaultLogger(globalOut);
    }

    ~LogInstance()
    {
        spdlog::shutdown();
    }
};

static bool RegisterQuitHandler()
{
#if defined(_WIN32)
    static auto CtrlHandler = ([](DWORD aType) {
        switch (aType)
        {
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT: {
            if (auto* pRunner = GetDediRunner())
            {
                pRunner->RequestKill();
                return TRUE;
            }
            // if the user kills during the ctor we deny the request
            // to save our dear life.
            return FALSE;
        }
        default:
            return FALSE;
        }
    });

    return SetConsoleCtrlHandler(CtrlHandler, TRUE);
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
        fs::create_directory(fs::current_path() / kConfigPathName);
        ShittyFileWrite(path, kEULAText);
        return false;
    }

    const auto data = TiltedPhoques::LoadFile(path);
    CSimpleIni si;
    if (si.LoadData(data.c_str()) != SI_OK)
        return false;

    return si.GetBoolValue("EULA", "bConfirmEULA", false);
}

int main(int argc, char** argv)
{
    // the binaries are not from the same commit.
    if (std::strcmp(GetBuildTag(), kBuildTag) != 0)
    {
        return 1;
    }

    LogInstance logger;
    (void)logger;

    if (!IsEULAAccepted())
    {
        spdlog::error("Please accept the EULA by setting bConfirmEULA to true in EULA.txt");
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
