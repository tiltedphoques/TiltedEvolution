#pragma once

/**
* @brief Dispatched when a remote actor is spawned or despawned locally.
*/
struct RemotePlayerSpawnedEvent
{
    RemotePlayerSpawnedEvent(uint32_t aPlayerId, bool aSpawned) : PlayerId(aPlayerId), Spawned(aSpawned)
    {
    }

    uint32_t PlayerId{};
    bool Spawned{};
};
