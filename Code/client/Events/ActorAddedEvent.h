#pragma once

struct ActorAddedEvent
{
    explicit ActorAddedEvent(const uint32_t aFormId)
        : FormId(aFormId)
    {}

    uint32_t FormId;
};
