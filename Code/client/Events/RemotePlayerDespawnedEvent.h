#pragma once

/**
* @brief Dispatched when a remote player actor is despawned locally.
*/
struct RemotePlayerDespawnedEvent
{
    RemotePlayerDespawnedEvent(uint32_t aPlayerId) 
        : PlayerId(aPlayerId)
    {
    }

    uint32_t PlayerId{};
};
