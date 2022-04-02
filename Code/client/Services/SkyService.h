#pragma once

#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct World;
struct ServerSkySeed;
struct DisconnectedEvent;
struct UpdateEvent;

class SkyService final
{
  public:
    SkyService(World&, entt::dispatcher&);

    static bool UseOnlineTick() noexcept;

    uint32_t GetWeatherSeed() const noexcept
    {
        return m_WeatherSeed;
    }

  private:
    void HandleUpdate(const UpdateEvent&) noexcept;
    void OnSeedAssignment(const ServerSkySeed&) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void OnDraw() noexcept;

    World& m_world;
    uint32_t m_WeatherSeed{};
    static bool s_mOnline;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_drawConnection;
    entt::scoped_connection m_seedConnection;
};
