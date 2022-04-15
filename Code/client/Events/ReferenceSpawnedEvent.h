#pragma once

// Not to be confused with added event, this event represents a form that the mod created
struct ReferenceSpawnedEvent
{
    explicit ReferenceSpawnedEvent(const uint32_t aFormId, const FormType aFormType, const entt::entity aEntity)
        : FormId(aFormId)
        , FormType(aFormType)
        , Entity(aEntity)
    {}

    uint32_t FormId;
    FormType FormType;
    entt::entity Entity;
};
