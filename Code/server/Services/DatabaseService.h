#pragma once

#include "DbPool.hpp"

struct World;
struct ObjectUpdatedEvent;
struct ObjectAddedEvent;

/**
* @brief Manages database interaction to persist the world.
*/
struct DatabaseService
{
    DatabaseService(World& aWorld, entt::dispatcher& aDispatcher);

private:

    void LoadObjects();

    void OnObjectUpdatedEvent(const ObjectUpdatedEvent& acEvent) noexcept;
    void OnObjectAddedEvent(const ObjectAddedEvent& acEvent) noexcept;

    World &m_world;
    Async::DbPool m_db;

    entt::scoped_connection m_objectUpdatedConnection;
    entt::scoped_connection m_objectAddedConnection;
};
