#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Actor.h>

struct PlayerCharacter : Actor
{
    static PlayerCharacter* Get() noexcept;

    uint8_t padPlayerCharacter[0xE10 - sizeof(Actor)];
};

#endif
