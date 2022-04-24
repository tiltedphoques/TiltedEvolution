#pragma once

#include <Events/PacketEvent.h>

struct World;
struct PlayerLeaveCellEvent;
struct ActivateRequest;
struct LockChangeRequest;
struct AssignObjectsRequest;
struct ScriptAnimationRequest;

/**
* @brief Manages (interactive) objects and relays interactions with said objects.
*/
class ObjectService
{
public:
    ObjectService(World &aWorld, entt::dispatcher &aDispatcher);

private:
    void OnPlayerLeaveCellEvent(const PlayerLeaveCellEvent& acEvent) noexcept;
    void OnAssignObjectsRequest(const PacketEvent<AssignObjectsRequest>&) noexcept;
    void OnActivate(const PacketEvent<ActivateRequest>&) const noexcept;
    void OnLockChange(const PacketEvent<LockChangeRequest>&) const noexcept;
    void OnScriptAnimationRequest(const PacketEvent<ScriptAnimationRequest>&) noexcept;

    World &m_world;

    entt::scoped_connection m_leaveCellConnection;
    entt::scoped_connection m_assignObjectConnection;
    entt::scoped_connection m_activateConnection;
    entt::scoped_connection m_lockChangeConnection;
    entt::scoped_connection m_scriptAnimationConnection;
};
