#include <Services/LocalServerRunnerService.h>
#include <base/threading/ThreadUtils.h>

namespace
{
constexpr wchar_t kServerDllName[] =
#ifdef TP_SKYRIM
    L"STServer.dll"
#elif TP_FALLOUT
    L"FTServer.dll"
#endif
    ;

constexpr char kBuildTag[]{BUILD_BRANCH "@" BUILD_COMMIT};

using TCheckBuildTag = bool (*)(const char*);
using TCreateServer = TiltedPhoques::UniquePtr<IGameServerInstance> (*)(Console::ConsoleRegistry&,
                                                                        const std::function<void()>&);

TCheckBuildTag g_pCheckTag{nullptr};
TCreateServer g_pCreateServer{nullptr};
} // namespace

LocalServerService::LocalServerService() : m_registry(spdlog::default_logger())
{
}

LocalServerService::~LocalServerService()
{
    if (m_pServerInstance)
        Kill();
}

bool LocalServerService::LoadServer()
{
    m_pServerDllHandle = LoadLibraryW(kServerDllName);
    if (!m_pServerDllHandle)
        return false;

    // pointers already set
    if (g_pCheckTag)
        return true;

    g_pCheckTag = reinterpret_cast<TCheckBuildTag>(GetProcAddress(m_pServerDllHandle, "?CheckBuildTag@@YA_NPEBD@Z"));

    if (!g_pCheckTag)
    {
        spdlog::error("Unable to locate export in ServerDLL");
        return false;
    }

    if (!g_pCheckTag(kBuildTag))
    {
        spdlog::error("Unable to start server: serverdll is from another build");
        return false;
    }

    // this is safe because a) the func prototype for CheckBuildTag may never change, so its call should always
    // succeed, so we can be sure that we are built from the same compile on the same arch on the same version.
    g_pCreateServer = reinterpret_cast<TCreateServer>(GetProcAddress(
        m_pServerDllHandle, "?CreateGameServer@@YA?AV?$unique_ptr@VIGameServerInstance@@U?$UniqueDeleter@"
                            "VIGameServerInstance@@@details@"
                            "TiltedPhoques@@@std@@AEAVConsoleRegistry@Console@@AEBV?$function@$$A6AXXZ@2@@Z"));

    if (!g_pCreateServer)
    {
        spdlog::error("Unable to start server: CreateGameServer() is missing");
        return false;
    }
    return true;
}

bool LocalServerService::Start()
{
    if (!LoadServer())
        return false;

    int argc = 0;
    char** argv = nullptr;

    m_pServerInstance = std::move(g_pCreateServer(m_registry, [this, argc, argv]() {}));
    m_thread = std::thread([&]() {
        m_thread.detach();
        Base::SetThreadName(m_thread.native_handle(), "STLanServerThread");
        m_started = true;
        this->Run();
    });
    return true;
}

void LocalServerService::Run()
{
    m_done = false;

    if (!m_pServerInstance)
        return;
    // this must happen on the net thread.
    if (!m_pServerInstance->Initialize())
        return;

    while (m_pServerInstance->IsListening())
        m_pServerInstance->Update();

    m_done = true;
}

void LocalServerService::Kill()
{
    m_pServerInstance->Shutdown();

    // yield till the server is done shutting down
    while (!m_done)
    {
    }

    m_started = false;
    m_pServerInstance.reset();
}
