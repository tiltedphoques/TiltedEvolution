
#include <cxxopts.hpp>
#include <Debug.hpp>

#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Initializer.hpp>

#include "Launcher.h"
#include "SteamSupport.h"

#include "loader/ExeLoader.h"
#include <BuildInfo.h>
#include "Utils/Error.h"

Launcher* g_pLauncher = nullptr;
constexpr uintptr_t kGameLoadLimit = 0x140000000 + 0x70000000;

extern void BootstrapGame(Launcher* apLauncher);

Launcher* GetLauncher() 
{
    return g_pLauncher;
}

Launcher::Launcher(int argc, char** argv)
{
    using TiltedPhoques::Debug;
    // only creates a new console if we aren't started from one
    Debug::CreateConsole();

    cxxopts::Options options(argv[0], R"(Welcome to the TiltedOnline command line \(^_^)/)");

    std::string gameName = "";
    options.add_options()
        ("h,help", "Display the help message")
        ("v,version", "Display the build version")
        ("g,game", "game name (SkyrimSE or Fallout4)", cxxopts::value<std::string>(gameName))
        ("r,reselect", "Reselect the game path");
    try
    {
        const auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            fmt::print(options.help({""}));
            m_appState = AppState::kFailed;
            return;
        }

        if (result.count("version"))
            fmt::print("TiltedOnline version: " BUILD_BRANCH "@" BUILD_COMMIT "\n");

        if (!gameName.empty())
        {
            if ((m_titleId = ToTitleId(gameName)) == TitleId::kUnknown)
            {
                fmt::print("Unable to determine game type\n");
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

    g_pLauncher = this;
}

Launcher::~Launcher()
{
    // explicit
    if (m_pClientHandle)
        FreeLibrary(m_pClientHandle);

    g_pLauncher = nullptr;
}

const fs::path& Launcher::GetGamePath() const
{
    return m_gamePath;
}

const fs::path& Launcher::GetExePath() const
{
    return m_exePath;
}

bool Launcher::Initialize()
{
    // there has been an error during startup
    if (m_appState == AppState::kFailed)
    {
        return false;
    }

    // no further initialization needed
    if (m_appState == AppState::kInGame)
    {
        return true;
    }

    // TBD: shared window + context init here
    return true;
}

void Launcher::InitPathEnvironment() noexcept
{
    auto appPath = TiltedPhoques::GetPath();
    SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);
    AddDllDirectory(appPath.c_str());
    AddDllDirectory(m_gamePath.c_str());
    SetCurrentDirectoryW(m_gamePath.c_str());

     std::wstring pathBuf;
    pathBuf.resize(32768);
    GetEnvironmentVariableW(L"PATH", pathBuf.data(), static_cast<DWORD>(pathBuf.length()));

    // append bin & game directories
    std::wstring newPath = appPath.native() + L";" + m_gamePath.native() + L";" + pathBuf;
    SetEnvironmentVariableW(L"PATH", newPath.c_str());
}

void Launcher::StartGame(TitleId aTid)
{
    // if the title id isn't unknown launch params take precedence, but
    // this shouldn't happen
    if (m_titleId == TitleId::kUnknown)
        m_titleId = aTid;

    ExeLoader::TEntryPoint start = nullptr;
    {
        if (!FindTitlePath(m_titleId, m_bReselectFlag, m_gamePath, m_exePath))
            return;

        InitPathEnvironment();
        SteamLoad(m_titleId, m_gamePath);

        ExeLoader loader(kGameLoadLimit, GetProcAddress);
        if (!loader.Load(m_exePath))
            return;

        start = loader.GetEntryPoint();
    }

    Initializer::RunAll();
    start();
}

void Launcher::LoadClient() noexcept
{
    WString clientName = ToClientName(m_titleId);
    fs::path clientDll = TiltedPhoques::GetPath() / clientName;
    fs::path cefDll = TiltedPhoques::GetPath() / L"libcef.dll";

    if (!(m_pClientHandle = LoadLibraryW(clientDll.c_str())))
    {
        if (!fs::exists(cefDll))
        {
            auto msg = fmt::format(L"Failed to load client\nLooks like CEF dlls are missing!\nPath: {}\nSuggested fix: "
                                   L"Run xmake install -o package", clientDll.native());
            FatalError(msg.c_str());
        }
        else
        {
            auto msg = fmt::format(L"Failed to load client\nPath: {}", clientDll.native());
            FatalError(msg.c_str());
        }

        TerminateProcess(GetCurrentProcess(), 0);
    }
}

int32_t Launcher::Exec() noexcept
{
    if (m_appState == AppState::kInGame)
    {
        StartGame(m_titleId);
        return 0;
    }

    // temporary selection code, until we have the new ui:
    fmt::print("Select game:\n""1) SkyrimSE\n2) Fallout4\n");

    TitleId tid{TitleId::kUnknown};

    int c;
    while (c = std::getchar())
    {
        switch (c)
        {
        case '1':
            tid = TitleId::kSkyrimSE;
            break;
        case '2':
            tid = TitleId::kFallout4;
            break;
        case '\n':
            break;
        default:
            fmt::print("Invalid selection. Try again!\n");
            break;
        }

        if (tid != TitleId::kUnknown)
            break;
    }

    StartGame(tid);
    return 0;
}
