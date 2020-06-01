#pragma once

struct ReferenceAddedEvent
{
    explicit ReferenceAddedEvent(const uint32_t aFormId, const uint8_t aFormType)
        : FormId(aFormId)
        , FormType(aFormType)
    {}

    uint32_t FormId;
    uint8_t FormType;
};
