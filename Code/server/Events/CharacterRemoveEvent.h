#pragma once

struct CharacterRemoveEvent
{
    CharacterRemoveEvent(const uint32_t aServerId)
        : ServerId(aServerId)
    {}

    uint32_t ServerId;
};
