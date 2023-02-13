#pragma once

#include <common/GameServerInstance.h>
#include <console/ConsoleRegistry.h>

struct LocalServerService
{
    LocalServerService();
    ~LocalServerService();

    bool Start();
    void Kill();

    bool IsRunning() const noexcept
    {
        return m_started;
    }
  private:
    void Run();
    bool LoadServer();

    HMODULE m_pServerDllHandle{nullptr};
    Console::ConsoleRegistry m_registry;
    TiltedPhoques::UniquePtr<IGameServerInstance> m_pServerInstance;
    std::thread m_thread;
    bool m_started{false};
    std::atomic_bool m_done{false};
};
