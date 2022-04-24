#pragma once

struct ActorRemovedEvent
{
    explicit ActorRemovedEvent(const uint32_t aFormId)
        : FormId(aFormId)
    {}

    uint32_t FormId;
};
