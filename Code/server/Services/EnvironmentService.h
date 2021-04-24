#pragma once

#include <Events/PacketEvent.h>
#include <Structs/TimeModel.h>
#include <Structs/GameId.h>

struct World;
struct UpdateEvent;
struct PlayerJoinEvent;
struct ActivateRequest;
struct LockChangeRequest;
struct AssignObjectsRequest;

class EnvironmentService
{
public:
    EnvironmentService(World &aWorld, entt::dispatcher &aDispatcher);

    // we use these types for SOL
    // this is done this way because SOL
    // provides direct support for these
    using TTime = std::pair<int, int>;
    using TDate = std::tuple<int, int, int>;

    bool SetTime(int aHour, int aMinutes, float aScale) noexcept;

    // returns hours, minutes
    TTime GetTime() const noexcept;
    static TTime GetRealTime() noexcept;

    // returns dd/mm/yy
    TDate GetDate() const noexcept;

    float GetTimeScale() const noexcept { return m_timeModel.TimeScale; }

private:
    void OnUpdate(const UpdateEvent &) noexcept; 
    void OnPlayerJoin(const PlayerJoinEvent&) const noexcept;
    void OnAssignObjectsRequest(const PacketEvent<AssignObjectsRequest>&) noexcept;
    void OnActivate(const PacketEvent<ActivateRequest>&) const noexcept;
    void OnLockChange(const PacketEvent<LockChangeRequest>&) const noexcept;

    TimeModel m_timeModel;
    uint64_t m_lastTick = 0;
    Map<GameId, entt::entity> m_objectsWithLocks;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_joinConnection;
    entt::scoped_connection m_assignObjectConnection;
    entt::scoped_connection m_activateConnection;
    entt::scoped_connection m_lockChangeConnection;
    World &m_world;
};
