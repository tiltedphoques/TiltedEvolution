#pragma once

struct ReferenceAddedEvent;
struct ReferenceRemovedEvent;
struct ReferenceSpawnedEvent;

struct Actor;
struct World;

struct EntityService
{
    EntityService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~EntityService() noexcept = default;

    TP_NOCOPYMOVE(EntityService);

    void OnReferenceAdded(const ReferenceAddedEvent&) noexcept;
    void OnReferenceRemoved(const ReferenceRemovedEvent&) noexcept;
    void OnReferenceSpawned(const ReferenceSpawnedEvent&) noexcept;

    [[nodiscard]] Outcome<entt::entity, bool> GetCharacter(uint32_t acFormId) const noexcept;

private:

    World& m_world;
    entt::dispatcher& m_dispatcher;

    Map<uint32_t, entt::entity> m_refIdToEntity;
    Map<uint32_t, entt::entity> m_spawnedEntities;

    entt::scoped_connection m_referenceAddedConnection;
    entt::scoped_connection m_referenceRemovedConnection;
    entt::scoped_connection m_referenceSpawnedConnection;
};
