
#include <cxxopts.hpp>
#include <Debug.hpp>

#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Initializer.hpp>

#include "Launcher.h"
#include "loader/ExeLoader.h"

#include "SteamSupport.h"

namespace fs = std::filesystem;

constexpr uintptr_t kGameLimit = 0x140000000 + 0x70000000;

extern bool BootstrapGame(Launcher*);

Launcher::Launcher(int argc, char** argv)
{
    using TiltedPhoques::Debug;
    // only creates a new console if we aren't started from one
    Debug::CreateConsole();

    cxxopts::Options options(argv[0], "Welcome to the TiltedOnline command line (^_^)/");

    std::string gameName = "";
    options.add_options()
        ("g,game", "game name", cxxopts::value<std::string>(gameName))
        ("r,reselect", "Reselect the game path");
    try
    {
        const auto result = options.parse(argc, argv);
        if (!gameName.empty())
        {
            if ((m_titleId = ToTitleId(gameName)) == TitleId::kUnknown)
            {
                fmt::print("Unable to determine game type");
                m_appState = AppState::kFailed;
                return;
            }

            // signal that we don't want the ui
            m_appState = AppState::kInGame;
        }

        m_bReselectFlag = result.count("reselect");
    }
    catch (const cxxopts::OptionException& ex)
    {
        m_appState = AppState::kFailed;
        fmt::print("Exception while parsing options: {}\n", ex.what());
    }
}

Launcher::~Launcher()
{
    // explicit
    if (m_pGameClientHandle)
        FreeLibrary(m_pGameClientHandle);
}

std::filesystem::path& Launcher::GetGamePath()
{
    return m_gamePath;
}

bool Launcher::Initialize()
{
    // there has been an error during startup
    if (m_appState == AppState::kFailed)
    {
        return false;
    }

    // jump directly in game
    if (m_appState == AppState::kInGame)
    {
        StartGame(m_titleId);
        return true;
    }

    return true;
}

void Launcher::StartGame(TitleId aTid)
{
    // if the title id isn't unknown launch params take precedence, but
    // this shouldn't happen
    if (m_titleId == TitleId::kUnknown)
        m_titleId = aTid;

    ExeLoader::entrypoint_t start = nullptr;
    {
        fs::path exeName;
        bool result = FindTitlePath(m_titleId, m_bReselectFlag, m_gamePath, exeName);

        auto fullPath = m_gamePath / exeName;
        if (!result || !fs::exists(fullPath))
        {
            return;
        }

        if (!BootstrapGame(this))
        {
            return;
        }

        SteamLoad(m_titleId, m_gamePath);

        ExeLoader loader(kGameLimit, GetProcAddress);
        if (!loader.Load(fullPath))
            return;

        start = loader.GetEntryPoint();
    }

    // apply all game hooks
    Initializer::RunAll();

    // this starts the game
    start();
}

void Launcher::LoadClient()
{
    WString clientName = ToClientName(m_titleId);

    auto clientPath = TiltedPhoques::GetPath() / clientName;
    m_pGameClientHandle = LoadLibraryW(clientPath.c_str());

    if (!m_pGameClientHandle)
    {
        auto errMsg = fmt::format(L"Unable to load the client!\n({})", clientPath.native());

        MessageBoxW(nullptr, errMsg.c_str(), L"TiltedOnline", MB_OK);
        //TerminateProcess(GetCurrentProcess(), 0);
    }
}

int32_t Launcher::Exec()
{
    // temporary anyway
    std::puts("Select game:\n""1) SkyrimSE\n2) Fallout4");

    TitleId tid{TitleId::kUnknown};

    int result = getchar();
    switch (result)
    {
    case '1':
        tid = TitleId::kSkyrimSE;
        break;
    case '2':
        tid = TitleId::kFallout4;
        break;
    default:
        return 0;
    }

    StartGame(tid);
    return 0;
}
