#pragma once

struct ReferenceRemovedEvent
{
    explicit ReferenceRemovedEvent(const uint32_t aFormId)
        : FormId(aFormId)
    {}

    uint32_t FormId;
};
