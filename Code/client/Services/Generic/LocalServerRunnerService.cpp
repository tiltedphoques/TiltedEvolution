#include <Services/LocalServerRunnerService.h>

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

LocalServerService::LocalServerService() : m_registry("basic_ass_logger")
{
}

void LocalServerService::Start()
{
    m_pServerDllHandle = LoadLibraryW(kServerDllName);
    if (!m_pServerDllHandle)
        return;

    if (!g_pCheckTag)
    {
        g_pCheckTag =
            reinterpret_cast<TCheckBuildTag>(GetProcAddress(m_pServerDllHandle, "?CheckBuildTag@@YA_NPEBD@Z"));

        if (!g_pCheckTag)
        {
            spdlog::error("Unable to locate export in ServerDLL");
            return;
        }

        if (!g_pCheckTag(kBuildTag))
        {
            spdlog::error("Unable to start server: serverdll is from another build");
            return;
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
            return;
        }
    }
    
    int argc = 0;
    char** argv = nullptr;

    m_pServerInstance = std::move(g_pCreateServer(m_registry, [this, argc, argv]() {}));
    m_pServerInstance->Initialize();

    m_thread = std::thread([this]() { this->Run(); });
}

void LocalServerService::Run()
{
    m_done = false;

    if (!m_pServerInstance)
        return;
    if (!m_pServerInstance->Initialize())
        return;

    while (m_pServerInstance->IsListening())
        m_pServerInstance->Update();

    m_done = true;
}

void LocalServerService::Kill()
{
    m_pServerInstance->Shutdown();

    while (!m_done)
    {
    }

    m_pServerInstance.reset();
}
