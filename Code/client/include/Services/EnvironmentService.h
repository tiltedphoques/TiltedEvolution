#pragma once

struct ServerTimeSettings;
struct TimeData;

class EnvironmentService final 
{
public:
    EnvironmentService(entt::dispatcher &);

private:
    void OnTimeUpdate(const ServerTimeSettings &);
    void HandleUpdate(const UpdateEvent &) noexcept;
    void OnDisconnected(const DisconnectedEvent &) noexcept;

    entt::scoped_connection m_timeUpdateConnection;
    entt::scoped_connection m_weatherUpdateConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;

    bool m_enableMPtime = false;
    float m_timeScale = 20.f;
    float m_time = -1.f;
    int m_year = 0;
    int m_day = 0;
    int m_month = 0;
};
