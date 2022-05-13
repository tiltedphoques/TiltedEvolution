#pragma once

/**
* @brief Dispatched when a remote player actor is spawned locally.
*/
struct RemotePlayerSpawnedEvent
{
    RemotePlayerSpawnedEvent(uint32_t aFormId, uint32_t aPlayerId) 
        : FormId(aFormId), PlayerId(aPlayerId)
    {
    }

    uint32_t FormId{};
    uint32_t PlayerId{};
};
