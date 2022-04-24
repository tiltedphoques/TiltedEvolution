#pragma once

/**
* @brief Dispatched whenever an actor has been unloaded from the environment.
*/
struct ActorRemovedEvent
{
    explicit ActorRemovedEvent(const uint32_t aFormId)
        : FormId(aFormId)
    {}

    uint32_t FormId;
};
