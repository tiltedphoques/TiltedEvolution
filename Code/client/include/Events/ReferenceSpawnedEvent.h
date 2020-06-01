#pragma once

// Not to be confused with added event, this event represents a form that the mod created
struct ReferenceSpawnedEvent
{
    explicit ReferenceSpawnedEvent(const uint32_t aFormId, const uint8_t aFormType, const entt::entity aEntity)
        : FormId(aFormId)
        , FormType(aFormType)
        , Entity(aEntity)
    {}

    uint32_t FormId;
    uint8_t FormType;
    entt::entity Entity;
};
