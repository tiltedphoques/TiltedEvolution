#pragma once

struct UpdateEvent;
struct World;

/**
* @brief Handles string caching replication
*/
struct StringCacheService
{
    StringCacheService(World& aWorld, entt::dispatcher& aDispatcher);

protected:

    void HandleUpdate(const UpdateEvent&) const noexcept;

private:
    World& m_world;

    entt::scoped_connection m_updateConnection;
};
