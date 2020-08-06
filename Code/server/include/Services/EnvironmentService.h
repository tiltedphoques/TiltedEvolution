#pragma once

#include <Events/PacketEvent.h>

struct World;
struct UpdateEvent;
struct PlayerJoinEvent;

class EnvironmentService
{
public:
    EnvironmentService(World &aWorld, entt::dispatcher &aDispatcher);

    bool SetTime(int, int, float);
    std::pair<int, int> GetTime();
    std::tuple<int, int, int> GetDate();
    float GetTimeScale() const { return m_timeScale; }

private:
    void OnUpdate(const UpdateEvent &) noexcept; 
    void OnPlayerJoin(const PlayerJoinEvent&) noexcept;

    // Default time: 01/01/01 at 12:00
    float m_timeScale = 20.f;
    float m_time = 12.f;
    int m_year = 1;
    int m_month = 1;
    int m_day = 1;

    uint64_t m_lastTick = 0;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_joinConnection;
    World &m_world;
};
