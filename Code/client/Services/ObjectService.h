#pragma once

#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct ServerTimeSettings;
struct DisconnectedEvent;
struct World;
struct ActivateEvent;
struct TransportService;
struct NotifyActivate;
struct LockChangeEvent;
struct NotifyLockChange;
struct CellChangeEvent;
struct ScriptAnimationEvent;
struct AssignObjectsResponse;
struct NotifyScriptAnimation;

/**
* @brief Handles objects in the environment.
*/
class ObjectService final 
    : public BSTEventSink<TESActivateEvent>
{
public:
    ObjectService(World&, entt::dispatcher&, TransportService&);

private:
    void OnDisconnected(const DisconnectedEvent &) noexcept;
    void OnCellChange(const CellChangeEvent &) noexcept;
    void OnAssignObjectsResponse(const AssignObjectsResponse &) noexcept;
    void OnActivate(const ActivateEvent &) noexcept;
    void OnActivateNotify(const NotifyActivate &) noexcept;
    void OnLockChange(const LockChangeEvent &) noexcept;
    void OnLockChangeNotify(const NotifyLockChange &) noexcept;
    void OnScriptAnimationEvent(const ScriptAnimationEvent &) noexcept;
    void OnNotifyScriptAnimation(const NotifyScriptAnimation &) noexcept;

    BSTEventResult OnEvent(const TESActivateEvent*, const EventDispatcher<TESActivateEvent>*) override;

    entt::entity CreateObjectEntity(const uint32_t acFormId, const uint32_t acServerId) noexcept;

    World& m_world;
    TransportService& m_transport;

    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_cellChangeConnection;
    entt::scoped_connection m_onActivateConnection;
    entt::scoped_connection m_activateConnection;
    entt::scoped_connection m_lockChangeConnection;
    entt::scoped_connection m_lockChangeNotifyConnection;
    entt::scoped_connection m_assignObjectConnection;
    entt::scoped_connection m_scriptAnimationConnection;
    entt::scoped_connection m_scriptAnimationNotifyConnection;
};
