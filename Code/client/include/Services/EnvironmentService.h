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
    void OnConnected(const ConnectedEvent &) const noexcept;
    void OnDisconnected(const DisconnectedEvent &) noexcept;

    entt::scoped_connection m_TimeUpdateConnection;
    entt::scoped_connection m_WeatherUpdateConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;

    float m_fTimeScale;
    float m_fTime = -1.f;
    int m_iYear = 0;
    int m_iDay = 0;
    int m_iMonth = 0;
};
