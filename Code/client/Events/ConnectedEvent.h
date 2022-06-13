#pragma once

/**
* @brief Dispatched when the client initially connects to the server.
*/
struct ConnectedEvent
{
    ConnectedEvent(uint32_t aPlayerId) : PlayerId(aPlayerId)
    {
    }

    uint32_t PlayerId{};
};
