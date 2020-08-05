#pragma once

#include <Events/PacketEvent.h>

struct World;
struct UpdateEvent;
struct CharacterSpawnedEvent;

class EnvironmentService
{
public:
    EnvironmentService(World &aWorld, entt::dispatcher &aDispatcher);

    bool SetTime(int, int, float);
    std::pair<int, int> GetTime();
    float GetTimeScale() const { return m_fTimeScale; }

private:
    void OnUpdate(const UpdateEvent &) noexcept; 
    void OnCharSpawn(const CharacterSpawnedEvent &) noexcept;

    // default skyrim values
    float m_fTimeScale = 20.f;
    float m_fTime = 0.f;
    int m_iYear = 0;
    int m_iDay = 0;
    int m_iMonth = 0;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_joinConnection;
    World &m_World;
};
