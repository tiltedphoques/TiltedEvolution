#pragma once

#include <Services/RunnerService.h>
#include <Services/TransportService.h>
#include <Services/PartyService.h>
#include <Services/OverlayService.h>

#include <Systems/ModSystem.h>


struct World : entt::registry
{
    World();
    ~World();

    void Update() noexcept;

    RunnerService& GetRunner() noexcept;
    TransportService& GetTransport() noexcept;
    ModSystem& GetModSystem() noexcept;

    const PartyService& GetPartyService() const noexcept { return ctx<const PartyService>(); }
    OverlayService& GetOverlayService() noexcept { return ctx<OverlayService>(); }
    const OverlayService& GetOverlayService() const noexcept { return ctx<const OverlayService>(); }

    auto &GetDispatcher() noexcept
    {
        return m_dispatcher;
    }

    [[nodiscard]] uint64_t GetTick() const noexcept;    

    static void Create() noexcept;
    [[nodiscard]] static World& Get() noexcept;

private:

    entt::dispatcher m_dispatcher;
    RunnerService m_runner;
    TransportService m_transport;
    ModSystem m_modSystem;

    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
};
