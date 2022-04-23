#pragma once

struct ReferenceAddedEvent;
struct ReferenceRemovedEvent;

struct Actor;
struct World;

/**
* @brief Responsible for managing (actor) entities.
*/
struct EntityService
{
    EntityService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~EntityService() noexcept = default;

    TP_NOCOPYMOVE(EntityService);

    /**
    * @brief Creates an entity for newly spawned actors.
    */
    void OnReferenceAdded(const ReferenceAddedEvent&) noexcept;
    /**
    * @brief Removes entities attached to removed actors.
    */
    void OnReferenceRemoved(const ReferenceRemovedEvent&) noexcept;

    /**
    * @brief Gets the entity attached to a form id.
    */
    [[nodiscard]] Outcome<entt::entity, bool> GetCharacter(uint32_t acFormId) const noexcept;

private:

    World& m_world;
    entt::dispatcher& m_dispatcher;

    //! @brief Maps form ids to entities.
    Map<uint32_t, entt::entity> m_formIdToEntity;

    entt::scoped_connection m_referenceAddedConnection;
    entt::scoped_connection m_referenceRemovedConnection;
    entt::scoped_connection m_referenceSpawnedConnection;
};
