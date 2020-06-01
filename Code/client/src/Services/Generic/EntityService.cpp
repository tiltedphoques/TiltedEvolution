#include <Services/EntityService.h>

#include <Events/ReferenceAddedEvent.h>
#include <Events/ReferenceRemovedEvent.h>
#include <Events/ReferenceSpawnedEvent.h>

#include <World.h>

#include <Components.h>

#include <Games/Fallout4/Forms/TESForm.h>
#include <Games/Skyrim/Forms/TESForm.h>

#include <Games/References.h>

EntityService::EntityService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
{
    m_referenceAddedConnection = m_dispatcher.sink<ReferenceAddedEvent>().connect<&EntityService::OnReferenceAdded>(this);
    m_referenceRemovedConnection = m_dispatcher.sink<ReferenceRemovedEvent>().connect<&EntityService::OnReferenceRemoved>(this);
    m_referenceSpawnedConnection = m_dispatcher.sink<ReferenceSpawnedEvent>().connect<&EntityService::OnReferenceSpawned>(this);
}

void EntityService::OnReferenceAdded(const ReferenceAddedEvent& acEvent) noexcept
{
    if (acEvent.FormType == Character)
    {
        entt::entity entity;

        const auto itor = m_spawnedEntities.find(acEvent.FormId);
        if (itor != std::end(m_spawnedEntities))
        {
            auto pForm = static_cast<Actor*>(TESForm::GetById(acEvent.FormId));
            pForm->GetExtension()->SetRemote(true);

            entity = itor->second;
            m_spawnedEntities.erase(itor);
        }
        else
            entity = m_world.create();

        m_world.emplace<FormIdComponent>(entity, acEvent.FormId);

        m_refIdToEntity[acEvent.FormId] = entity;
    }
}

void EntityService::OnReferenceRemoved(const ReferenceRemovedEvent& acEvent) noexcept
{
    const auto cIterator = m_refIdToEntity.find(acEvent.FormId);

    if (cIterator != std::end(m_refIdToEntity))
    {
        const auto entity = cIterator->second;

        m_world.remove<FormIdComponent>(entity);

        if (m_world.orphan(entity))
            m_world.destroy(entity);

        m_refIdToEntity.erase(cIterator);
    }
}

void EntityService::OnReferenceSpawned(const ReferenceSpawnedEvent& acEvent) noexcept
{
    if (acEvent.FormType == Character)
    {
        m_spawnedEntities.emplace(acEvent.FormId, acEvent.Entity);
    }
}

Outcome<entt::entity, bool> EntityService::GetCharacter(const uint32_t acFormId) const noexcept
{
    const auto cPair = m_refIdToEntity.find(acFormId);
    if (cPair != std::end(m_refIdToEntity))
    {
        const auto entity = cPair->second;

        return entity;
    }

    return false;
}
