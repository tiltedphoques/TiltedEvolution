
#include "GameServer.h"
#include <TiltedCore/Stl.hpp>
#include <common/GameServerInstance.h>
#include <console/ConsoleRegistry.h>

#ifdef _WIN32
#define GS_EXPORT __declspec(dllexport)
#else
#define GS_EXPORT __attribute__((visibility("default")))
#endif

namespace
{
constexpr char kBuildTag[]{BUILD_BRANCH "@" BUILD_COMMIT};
} // namespace

class GameServerInstance final : public IGameServerInstance
{
  public:
    GameServerInstance(Console::ConsoleRegistry& aConsole) : m_gameServer(aConsole)
    {
    }

    // to make sure our dtor is called.
    ~GameServerInstance() override = default;

    // Inherited via IGameServerInstance
    virtual bool Initialize() override;
    virtual void Shutdown() override;
    virtual bool IsListening() override;
    virtual bool IsRunning() override;
    virtual void Update() override;

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

// memory is owned by the game server, use destroy to ensure destruction
GS_EXPORT UniquePtr<IGameServerInstance> CreateGameServer(Console::ConsoleRegistry& aConReg, void* apUserPointer,
                                                          void (*apCallback)(void*))
{
    // register static variables before they become available to the server
    aConReg.BindStaticItems();

    // this is a special callback to notify the runner once all settings become available
    apCallback(apUserPointer);

    return TiltedPhoques::CastUnique<IGameServerInstance>(TiltedPhoques::MakeUnique<GameServerInstance>(aConReg));
}

// cxx symbol
GS_EXPORT void SetDefaultLogger(std::shared_ptr<spdlog::logger> aLogger)
{
    spdlog::set_default_logger(std::move(aLogger));
}

GS_EXPORT void RegisterLogger(std::shared_ptr<spdlog::logger> aLogger)
{
    // yes this needs to be here, else the dedirunner dies
    spdlog::register_logger(std::move(aLogger));
}

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
