#pragma once

struct UpdateEvent;
struct ConnectedEvent;
struct DisconnectedEvent;
struct ReferenceSpawnedEvent;
struct FormIdComponent;
struct ActionEvent;
struct AssignCharacterResponse;

struct World;
struct TransportService;

struct CharacterService
{
    CharacterService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~CharacterService() noexcept = default;

    TP_NOCOPYMOVE(CharacterService);

    void OnFormIdComponentAdded(entt::registry& aRegistry, entt::entity aEntity) const noexcept;
    void OnFormIdComponentRemoved(entt::registry& aRegistry, entt::entity aEntity) const noexcept;
    void OnUpdate(const UpdateEvent& acUpdateEvent) noexcept;
    void OnConnected(const ConnectedEvent& acConnectedEvent) const noexcept;
    void OnDisconnected(const DisconnectedEvent& acDisconnectedEvent) const noexcept;
    void OnAssignCharacter(const AssignCharacterResponse& acMessage) noexcept;
    void OnCharacterSpawn(const TiltedMessages::CharacterSpawnRequest& acMessage) const noexcept;
    void OnReferenceMovementSnapshot(const TiltedMessages::ReferenceMovementSnapshot& acMessage) noexcept;
    void OnActionEvent(const ActionEvent& acActionEvent) noexcept;

private:

    void RequestServerAssignment(entt::registry& aRegistry, entt::entity aEntity) const noexcept;
    void CancelServerAssignment(entt::registry& aRegistry, entt::entity aEntity) const noexcept;

    void RunLocalUpdates() noexcept;
    void RunRemoteUpdates() noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    entt::scoped_connection m_formIdAddedConnection;
    entt::scoped_connection m_formIdRemovedConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_actionConnection;
    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_assignCharacterConnection;
    entt::scoped_connection m_characterSpawnConnection;
    entt::scoped_connection m_referenceMovementSnapshotConnection;
};
