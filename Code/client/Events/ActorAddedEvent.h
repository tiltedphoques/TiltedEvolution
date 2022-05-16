#pragma once

/**
* @brief Dispatched whenever a new loaded actor has been detected in the environment.
*/
struct ActorAddedEvent
{
    explicit ActorAddedEvent(const uint32_t aFormId)
        : FormId(aFormId)
    {}

    uint32_t FormId;
};
