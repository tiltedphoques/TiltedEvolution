
#include "GameServer.h"

#include <common/GameServerInstance.h>

#ifdef _WIN32
#define GS_EXPORT __declspec(dllexport)
#else
#define GS_EXPORT __attribute__((visibility("default")))
#endif

namespace
{
constexpr char kBuildTag[]{BUILD_BRANCH "@" BUILD_COMMIT};
} // namespace

struct GameServerInstance final : IGameServerInstance
{
    GameServerInstance(Console::ConsoleRegistry& aConsole) : m_gameServer(aConsole)
    {
    }

    // to make sure our dtor is called.
    ~GameServerInstance() override = default;

    // Inherited via IGameServerInstance
    bool Initialize() override;
    void Shutdown() override;
    bool IsListening() override;
    bool IsRunning() override;
    void Update() override;

private:
    GameServer m_gameServer;
};

bool GameServerInstance::Initialize()
{
    m_gameServer.Initialize();
    return true;
}

void GameServerInstance::Shutdown()
{
    m_gameServer.Kill();
}

bool GameServerInstance::IsListening()
{
    return m_gameServer.IsListening();
}

bool GameServerInstance::IsRunning()
{
    return m_gameServer.IsRunning();
}

void GameServerInstance::Update()
{
    m_gameServer.Update();
}

// NOTE(Vince): For now we use this to compare the dll to the server.
GS_EXPORT const char* GetBuildTag()
{
    return kBuildTag;
}

GS_EXPORT bool CheckBuildTag(const char* apBuildTag)
{
    return std::strcmp(apBuildTag, kBuildTag) == 0;
}

GS_EXPORT UniquePtr<IGameServerInstance> CreateGameServer(Console::ConsoleRegistry& aConReg, const std::function<void()>& aCallback)
{
    BASE_ASSERT(aCallback, "CreateGameServer(): Callback was not provided");

    // register static variables before they become available to the server
    aConReg.BindStaticItems();

    // this is a special callback to notify the runner once all settings become available
    aCallback();

    return TiltedPhoques::CastUnique<IGameServerInstance>(TiltedPhoques::MakeUnique<GameServerInstance>(aConReg));
}

// cxx symbol

// These two are windows specific implementation details, since all symbols & insances are private there, so we must hand over
// the control.
// if not compiling with -fvisibility=hidden, hide these

GS_EXPORT void SetDefaultLogger(std::shared_ptr<spdlog::logger> aLogger)
{
    //#ifdef _WIN32
    spdlog::set_default_logger(std::move(aLogger));
    //#endif
}

GS_EXPORT void RegisterLogger(std::shared_ptr<spdlog::logger> aLogger)
{
    //#ifdef _WIN32
    // yes this needs to be here, else the dedirunner dies
    spdlog::register_logger(std::move(aLogger));
    //#endif
}

#ifdef _WIN32
// Before you think about moving logic in here...
// There are significant limits on what you can safely do in a DLL entry point. See General Best Practices for specific
// Windows APIs that are unsafe to call in DllMain. If you need anything but the simplest initialization then do that in
// an initialization function for the DLL. You can require applications to call the initialization function after
// DllMain has run and before they call any other functions in the DLL.
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif
