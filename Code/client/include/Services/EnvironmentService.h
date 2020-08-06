#pragma once

struct ServerTimeSettings;
struct TimeData;

class EnvironmentService final 
{
public:
    EnvironmentService(World &, entt::dispatcher &);

private:
    void OnTimeUpdate(const ServerTimeSettings &);
    void HandleUpdate(const UpdateEvent &) noexcept;
    void OnDisconnected(const DisconnectedEvent &) noexcept;

    entt::scoped_connection m_timeUpdateConnection;
    entt::scoped_connection m_weatherUpdateConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;

    bool m_enableRemoteTimeModel = false;
    float m_timeScale = 20.f;
    float m_time = 0.f;
    int m_year = 1;
    int m_day = 1;
    int m_month = 1;

    uint64_t m_remoteTick = 0;
    uint64_t m_lastTick = 0;
    World& m_world;
};
