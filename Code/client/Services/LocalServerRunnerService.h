#pragma once

#include <common/GameServerInstance.h>
#include <console/ConsoleRegistry.h>

struct LocalServerService
{
    LocalServerService();

    void Start();
    void Kill();
    
  private:
    void Run();

    HMODULE m_pServerDllHandle{nullptr};
    Console::ConsoleRegistry m_registry;
    TiltedPhoques::UniquePtr<IGameServerInstance> m_pServerInstance;
    std::thread m_thread;
    std::atomic_bool m_done{false};
};
