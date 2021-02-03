#pragma once

#include <Structs/TimeModel.h>

struct ServerTimeSettings;
struct DisconnectedEvent;
struct World;
struct UpdateEvent;

class EnvironmentService final 
{
public:
    EnvironmentService(World&, entt::dispatcher&);

    static bool AllowGameTick() noexcept;
  private:
    void OnTimeUpdate(const ServerTimeSettings &) noexcept;
    void HandleUpdate(const UpdateEvent &) noexcept;
    void OnDisconnected(const DisconnectedEvent &) noexcept;

    void ToggleGameClock(bool aEnable);
    float TimeInterpolate(const TimeModel& aFrom, TimeModel& aTo) const;

    entt::scoped_connection m_timeUpdateConnection;
    entt::scoped_connection m_weatherUpdateConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;

    TimeModel m_onlineTime;
    TimeModel m_offlineTime;
    float m_fadeTimer = 0.f;
    bool m_switchToOffline = false;
    static bool s_gameClockLocked;

    uint64_t m_lastTick = 0;
    World& m_world;
};
