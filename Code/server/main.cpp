
#include "GameServer.h"

#define GS_EXPORT __declspec(dllexport)

namespace
{
constexpr char kBuildTag[]{BUILD_BRANCH "@" BUILD_COMMIT};

consteval int GetNumericBuildId()
{
    // constexpr const char* ptr = &BUILD_COMMIT[1];

    // TBD
    return 0x1337;
}
} // namespace

// NOTE(Vince): For now we use this to compare the dll to the server.
GS_EXPORT const char* GetBuildTag()
{
    return kBuildTag;
}

GS_EXPORT bool CompareBuildInfo(size_t aGsSize)
{
    return aGsSize == sizeof(GameServer);
}

// memory is owned by the game server, use destroy to ensure destruction
GS_EXPORT GameServer* CreateGameServer(Console::ConsoleRegistry& conReg)
{
    return new GameServer(conReg);
}

GS_EXPORT void DestroyGameServer(GameServer* apServer)
{
    delete apServer;
}

// cxx symbol
GS_EXPORT void SetDefaultLogger(std::shared_ptr<spdlog::logger> default_logger)
{
    spdlog::set_default_logger(std::move(default_logger));
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
