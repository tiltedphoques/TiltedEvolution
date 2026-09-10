#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Buffer.hpp>
#include <TiltedCore/Serialization.hpp>
#include <TiltedCore/Outcome.hpp>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <optional>

using TiltedPhoques::List;
using TiltedPhoques::Outcome;
using TiltedPhoques::Set;
using TiltedPhoques::Vector;

#include <Components.h>
#include <Systems/ObjectSystem.h>

entt::entity ObjectSystem::Setup(entt::registry& aRegistry, const uint32_t aFormId, const uint32_t aServerId) noexcept
{
    const auto view = aRegistry.view<FormIdComponent, ObjectComponent>();
    entt::entity existingEntity = entt::null;
    Vector<entt::entity> staleEntities;

    for (const auto entity : view)
    {
        const auto& formIdComponent = view.get<FormIdComponent>(entity);
        const auto& objectComponent = view.get<ObjectComponent>(entity);

        if (formIdComponent.Id == aFormId && existingEntity == entt::null)
            existingEntity = entity;
        else if (formIdComponent.Id == aFormId || objectComponent.Id == aServerId)
            staleEntities.push_back(entity);
    }

    for (const auto entity : staleEntities)
        aRegistry.destroy(entity);

    if (existingEntity != entt::null)
    {
        aRegistry.get<ObjectComponent>(existingEntity).Id = aServerId;
        return existingEntity;
    }

    const entt::entity entity = aRegistry.create();
    aRegistry.emplace<FormIdComponent>(entity, aFormId);
    aRegistry.emplace<ObjectComponent>(entity, aServerId);
    return entity;
}

void ObjectSystem::Remove(entt::registry& aRegistry, const std::span<const uint32_t> aServerIds) noexcept
{
    const Set<uint32_t> serverIds(aServerIds.begin(), aServerIds.end());
    const auto view = aRegistry.view<ObjectComponent>();
    Vector<entt::entity> entities;

    for (const auto entity : view)
    {
        if (serverIds.count(view.get<ObjectComponent>(entity).Id))
            entities.push_back(entity);
    }

    for (const auto entity : entities)
        aRegistry.destroy(entity);
}

void ObjectSystem::Clear(entt::registry& aRegistry) noexcept
{
    const auto view = aRegistry.view<ObjectComponent>();
    const Vector<entt::entity> entities(view.begin(), view.end());
    for (const auto entity : entities)
        aRegistry.destroy(entity);
}
