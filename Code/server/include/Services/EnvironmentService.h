#pragma once

#include <Events/PacketEvent.h>
#include <Structs/TimeModel.h>

struct World;
struct UpdateEvent;
struct PlayerJoinEvent;

class EnvironmentService
{
public:
    EnvironmentService(World &aWorld, entt::dispatcher &aDispatcher);

    struct Time
    {
        int Minutes;
        int Hours;
    };

    struct Date
    {
        int Day;
        int Month;
        int Year;
    };

    bool SetTime(int aHour, int aMinutes, float aScale) noexcept;
    Time GetTime() const noexcept;
    Date GetDate() const noexcept;
    float GetTimeScale() const noexcept { return m_timeModel.TimeScale; }

private:
    void OnUpdate(const UpdateEvent &) noexcept; 
    void OnPlayerJoin(const PlayerJoinEvent&) noexcept;

    TimeModel m_timeModel;
    uint64_t m_lastTick = 0;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_joinConnection;
    World &m_world;
};
