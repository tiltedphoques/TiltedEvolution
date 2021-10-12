#include <stdafx.h>

#include <Scripts/EntityHandle.h>
#include <World.h>

namespace Script
{
    EntityHandle::EntityHandle(entt::entity aEntity, World& aWorld)
        : m_entity(aEntity)
        , m_pWorld(&aWorld)
    {}

    uint32_t EntityHandle::GetId() const
    {
        return World::ToInteger(m_entity);
    }

    EntityHandle& EntityHandle::operator=(const EntityHandle& acRhs)
    {
        m_entity = acRhs.m_entity;
        m_pWorld = acRhs.m_pWorld;

        return *this;
    }
}
