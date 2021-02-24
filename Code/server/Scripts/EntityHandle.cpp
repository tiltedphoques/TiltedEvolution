#include <stdafx.h>

#include <Scripts/EntityHandle.h>
#include <World.h>
#include <NetObject.h>
#include <Components.h>

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

    bool EntityHandle::AddComponent(sol::object aObject) const
    {
        if (m_pWorld->valid(m_entity))
        {
            auto netObject = aObject.as<std::optional<NetObject::Pointer>>();
            if(netObject)
            {
                auto pNetObject = *netObject;

                if (pNetObject->SetParentId(GetId()))
                {
                    auto& scriptsComponent = m_pWorld->get<ScriptsComponent>(m_entity);
                    auto* pOwnerComponent = m_pWorld->try_get<OwnerComponent>(m_entity);
                    if(pOwnerComponent)
                    {
                        pNetObject->SetOwnerId(pOwnerComponent->ConnectionId);
                    }
                    scriptsComponent.Components.push_back(pNetObject);

                    return true;
                }
            }
        }

        return false;
    }
}
