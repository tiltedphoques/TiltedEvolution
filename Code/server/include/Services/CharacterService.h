#pragma once

#include <Events/PacketEvent.h>

struct UpdateEvent;
struct CharacterSpawnedEvent;
struct World;

struct CharacterService
{
    CharacterService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~CharacterService() noexcept = default;

    TP_NOCOPYMOVE(CharacterService);

    static void Serialize(const World& aRegistry, entt::entity aEntity, TiltedMessages::CharacterSpawnRequest* apSpawnRequest) noexcept;

protected:

    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnCharacterAssignRequest(const PacketEvent<TiltedMessages::CharacterAssignRequest>& acMessage) const noexcept;
    void OnCharacterSpawned(const CharacterSpawnedEvent& acEvent) noexcept;
    void OnReferenceMovementSnapshot(const PacketEvent<TiltedMessages::ReferenceMovementSnapshot>& acMessage) const noexcept;

    void CreateCharacter(const PacketEvent<TiltedMessages::CharacterAssignRequest>& acMessage) const noexcept;

private:

    World& m_world;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_characterAssignRequestConnection;
    entt::scoped_connection m_characterSpawnedConnection;
    entt::scoped_connection m_referenceMovementSnapshotConnection;
};
