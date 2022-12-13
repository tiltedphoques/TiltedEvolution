#pragma once

struct World;

namespace Script
{
// Derive from this if you want to expose an entity's content, we need to know the world the entity belongs to
struct EntityHandle
{
    virtual ~EntityHandle() = default;
    EntityHandle(entt::entity aEntity, World& aWorld);

    [[nodiscard]] uint32_t GetId() const;

    EntityHandle& operator=(const EntityHandle& acRhs);

protected:
    entt::entity m_entity;
    World* m_pWorld;
};
} // namespace Script
