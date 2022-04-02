#pragma once

#include <Services/EnvironmentService.h>

class SkyService
{
  public:
    SkyService(World& aWorld, entt::dispatcher& aDispatcher);

    uint64 GetWeatherSeed() const noexcept
    {
        return m_weatherSeed;
    }

  private:
    void OnUpdate(const UpdateEvent&) noexcept;
    void OnPlayerJoin(const PlayerJoinEvent&) const noexcept;

    World& m_world;
    EnvironmentService& m_envService;
    uint64_t m_lastTick = 0;
    uint64_t m_weatherSeed = 0;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_joinConnection;
    mutable std::chrono::steady_clock::time_point m_nextUpdate;
};
