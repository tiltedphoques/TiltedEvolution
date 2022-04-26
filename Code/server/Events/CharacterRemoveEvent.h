#pragma once

/**
* @brief Dispatched when an actor should be removed from the world.
* 
* This happens whenever a player leaves, and the player actor needs to be cleaned up on
* remote clients, or whenever there is no new owner to be found for an NPC.
*/
struct CharacterRemoveEvent
{
    CharacterRemoveEvent(const uint32_t aServerId)
        : ServerId(aServerId)
    {}

    uint32_t ServerId;
};
