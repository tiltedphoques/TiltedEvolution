#pragma once

#include <Events/PacketEvent.h>

struct UpdateEvent;
struct CharacterSpawnedEvent;
struct World;
struct AssignCharacterRequest;
struct CharacterSpawnRequest;
struct ClientReferencesMoveRequest;
struct RequestInventoryChanges;

struct CharacterService
{
    CharacterService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~CharacterService() noexcept = default;

    TP_NOCOPYMOVE(CharacterService);

    static void Serialize(const World& aRegistry, entt::entity aEntity, CharacterSpawnRequest* apSpawnRequest) noexcept;

protected:

    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnAssignCharacterRequest(const PacketEvent<AssignCharacterRequest>& acMessage) const noexcept;
    void OnCharacterSpawned(const CharacterSpawnedEvent& acEvent) noexcept;
    void OnReferencesMoveRequest(const PacketEvent<ClientReferencesMoveRequest>& acMessage) const noexcept;
    void OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) const noexcept;

    void CreateCharacter(const PacketEvent<AssignCharacterRequest>& acMessage) const noexcept;

    void ProcessInventoryChanges() noexcept;
    void ProcessMovementChanges() noexcept;

private:

    World& m_world;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_characterAssignRequestConnection;
    entt::scoped_connection m_characterSpawnedConnection;
    entt::scoped_connection m_referenceMovementSnapshotConnection;
    entt::scoped_connection m_inventoryChangesConnection;
};
