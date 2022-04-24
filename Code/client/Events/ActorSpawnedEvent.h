#pragma once

// Not to be confused with added event, this event represents a form that the mod created
struct ActorSpawnedEvent
{
    explicit ActorSpawnedEvent(const uint32_t aFormId, const entt::entity aEntity)
        : FormId(aFormId)
        , Entity(aEntity)
    {}

    uint32_t FormId;
    entt::entity Entity;
};
